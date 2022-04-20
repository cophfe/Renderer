#include "Program.h"
#include "MeshPrimitive.h"
#include "MeshRendererComponent.h"
#include "CameraMoveComponent.h"
#include <filesystem>
namespace fs = std::filesystem;

Program* Program::instance = nullptr;

Program* Program::CreateInstance()
{
	if (!instance)
		instance = new Program();
	return instance;
}

void Program::DestroyInstance()
{
	delete instance;
	instance = nullptr;
}

void Program::Run()
{
	Init();
	Loop();
	Cleanup();
}

void Program::RegisterGameObject(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);
	if (initiated)
		newGameObjects.push_back(gameObject);
}

void Program::DeregisterGameObject(GameObject* gameObject)
{
	auto position = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
	if (position != gameObjects.end())
		gameObjects.erase(position);
}

void Program::AddChild(Transform& child)
{
	if (child.GetParent() == nullptr)
		children.push_back(&child);
}

void Program::RemoveChild(Transform& child)
{
	if (child.GetParent() == nullptr)
	{
		auto position = std::find(children.begin(), children.end(), &child);
		if (position != children.end())
			children.erase(position);
	}
}

void Program::SetCursorLocked(bool cursorLocked)
{
	static bool cL = false;
	if (cL == cursorLocked)
		return;
	cL = cursorLocked;

	if (cursorLocked)
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

//quick thing to show examples of objects
//super inefficient and easy to break


void Program::Init()
{
	InitGraphics();
	InitCallbacks();

	double x, y;
	glfwGetCursorPos(GetWindow(), &x, &y);
	cursorPosition = Vector2(x, y);

	lastTime = glfwGetTime();
	shaderRecompileTimer = shaderRecompileTime;

	auto& tM = renderer.GetTextureManager();

	Shader* pbrFrag = Shader::InitNew("Shaders/PBR.frag", Shader::Type::Fragment);
	Shader* pbrVert = Shader::InitNew("Shaders/PBR.vert", Shader::Type::Vertex);

	//load in some meshes
	const int meshCount = 8;
							// Amogus	Ball	Car		Shiny	SpaceShip	Sphere	Table	TV
	float scale[meshCount] = { 1.0f,	10.0f,	1.0f,	1.0f,	1.0f,		1.0f,	0.03f,	0.1f };
	float offset[meshCount] ={ 1.0f,	1.0f,	1.0f,	1.0f,	2.7f,		2.5f,	1.0f,	1.7f };
	bool rotate[meshCount] = { true,	false,	false,	false,	false,		false,	false,	true };

	auto p = fs::path("Models/");
	int i = 0;
	float currentXOffset = -17.5f;
	for (const auto& file : fs::directory_iterator(p))
	{
		if (i >= meshCount)
			break;
		
		auto fileName = file.path();
		auto stem = fileName.stem().string();
		auto ext = fileName.extension().string();

		std::string ao = "";
		std::string albedo = "";
		std::string metal = "";
		std::string normal = "";
		std::string roughness = "";

		if (!(ext == ".fbx" || ext == ".obj"))
			continue;

		Mesh* mesh = MeshBuilder::LoadMeshFromPath(fileName.string().c_str());
		Material* mat = Material::InitNew(*pbrFrag, *pbrVert);

		auto texPath = fs::path("Textures/" + stem);
		auto filePath = stem + "/";
		for (const auto& tex : fs::directory_iterator(texPath))
		{
			auto texPath = tex.path();
			auto texStem = texPath.stem().string();
			auto texExt = texPath.extension().string();

			if (texExt != ".tga" && texExt != ".png" && texExt != ".jpg")
				continue;

			if (texStem == "ao")
				ao = filePath + texPath.filename().string();
			else if (texStem == "albedo")
				albedo = filePath + texPath.filename().string();
			else if (texStem == "metal")
				metal = filePath + texPath.filename().string();
			else if (texStem == "roughness")
				roughness = filePath + texPath.filename().string();
			else if (texStem == "normal")
				normal = filePath + texPath.filename().string();
		}
		Program::GetInstance()->GetRenderer().SetPBRValues(mat, 1.0f, 1.0f, albedo.c_str(), normal.c_str(), roughness.c_str(), metal.c_str(), ao.c_str(), true);
		GameObject* gO = GameObject::Create();
		gO->AddComponent<MeshRendererComponent>()->Init(mesh, mat);
		
		auto& t = gO->GetTransform();
		t.SetLocalPosition(Vector3(currentXOffset, -2.0f, -7.0f));
		t.SetLocalScale(scale[i]);
		if (rotate[i])
		{
			t.LocalRotate(glm::radians(-90.0f), Vector3(1, 0, 0));
		}
		i++;
		currentXOffset += 3.0f * offset[i];
		
	}

	//add stage for reference
	GameObject* stage = GameObject::Create();
	Material* stageMat = Material::InitNew(*pbrVert, *pbrFrag);
	MeshData box;
	MeshPrimitive::SetCube(box, 0.5f);
	Mesh* stageMesh = Mesh::InitNew(&box, 1);
	stage->AddComponent<MeshRendererComponent>()->Init(stageMesh, stageMat);
	stage->GetTransform().SetLocalPosition(Vector3(0, -4.0f, -7));
	stage->GetTransform().SetLocalScale(Vector3(40.0f, 0.5f, 8.0f));
	renderer.SetPBRValues(stageMat, 0.5f, 0.2f);
	initiated = true;
}

void Program::Start()
{
	srand((unsigned int)(std::time(0)));

	if (gameObjects.size() > 0)
	{
		for (auto* gameObject : newGameObjects)
		{
			gameObject->Start();
		}
		newGameObjects.clear();
	}
}

void Program::InitGraphics()
{
	renderer.Init("Textures/");
	renderer.GetTextureManager().internalFormat = GL_RGBA16F;
	//add sun
	GameObject* sunObj = GameObject::Create();
	auto* sun = sunObj->AddComponent<LightComponent>();
	sun->Init(Vector3(1, 0.9568627f, 0.8392157f) * 1.0f, LightType::DIRECTION);
	sunObj->GetTransform().SetRotation(glm::quatLookAt(glm::normalize(Vector3(0.4f, 5, -4.0f)), Vector3(0, 1, 0)));

	//setup camera
	Vector2Int size;
	glfwGetWindowSize(renderer.GetWindow(), &size.x, &size.y);
	auto* camera = GameObject::Create();
	camera->AddComponent<CameraComponent>()->Init(glm::radians(75.0f), size.x / (float)size.y);
	//add camera light
	auto* camLight = camera->AddComponent<LightComponent>();
	camLight->Init(Vector3(1, 0.9f, 0.8f) * 5.0f, 20.0f, glm::radians(30.0f), glm::radians(8.0f), LightType::SPOTLIGHT);
	//add mover
	camera->AddComponent<CameraMoveComponent>()->Init(3, 1.06f, 4);
}

void Program::InitCallbacks()
{
	GLFWwindow* window = renderer.GetWindow();

	glfwSetWindowSizeCallback(window, OnWindowResize);
	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetScrollCallback(window, OnMouseWheel);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetWindowFocusCallback(window, OnWindowFocus);
}

void Program::Loop()
{
	//start for the first gameobjects operates on the entire gameobject list
	for (auto* gameObject : gameObjects)
	{
		gameObject->Start();
	}

	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		Start();
		glfwPollEvents();
		UpdateTime();
		Update();
		Render();
	}
}

void Program::Render()
{
	renderer.UpdateUniformBuffers();
	renderer.Render();
}

void Program::Update()
{
	for (auto gameObject : gameObjects)
	{
		gameObject->Update();
	}
}

void Program::Cleanup()
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Unload();
		delete gameObjects[i];
	}
	gameObjects.clear();

	renderer.Cleanup();
}

void Program::UpdateTime()
{
	double newTime = glfwGetTime();
	deltaTime = newTime - lastTime;
	lastTime = newTime;
}

#pragma region Input 
void Program::KeyPressed(int key)
{
}

void Program::KeyReleased(int key)
{

}

void Program::MousePressed(int button)
{
}

void Program::MouseReleased(int button)
{
	if (button == 0)
	{
		static std::vector<LightComponent*> lCs; //pls ignore this line
		Vector3 lum = Vector3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);

		//this is a way to make a color highly saturated, yoinked from a javascript project of mine
		int min = 0, max = 0;
		if (lum.x > lum.y)
		{
			max = (lum.x > lum.z) ? 0 : 2;
			min = (lum.y < lum.z) ? 1 : 2;
		}
		else
		{
			max = (lum.y > lum.z) ? 1 : 2;
			float notmax = 1.0f + fmod(max, 2.0f);
			min = (lum.x < lum[notmax]) ? 0 : notmax;
		}
		lum[min] = 0;
		lum[max] = 1.0f;
		lum *= 10.0f;

		if (lCs.size() >= 6)
		{
			lCs[0]->SetLuminance(lum);
			lCs[0]->GetTransform().SetPosition(renderer.GetMainCamera()->GetTransform().GetPosition());
		}
		else {
			auto* gO = GameObject::Create();
			auto* lO = gO->AddComponent<LightComponent>();
			lO->Init(lum, 20.0f, LightType::POINT);
			gO->GetTransform().SetPosition(renderer.GetMainCamera()->GetTransform().GetPosition());
			lCs.push_back(lO);
		}
	}
}

void Program::MouseMove(Vector2 mousePosition)
{
	if (!windowFocused)
		return;

	cursorPosition = mousePosition;
}

void Program::MouseScroll(float delta)
{

}

void Program::WindowResize(Vector2Int size)
{
	renderer.OnWindowResize(size);

}
void Program::WindowFocus(bool focus)
{
	windowFocused = focus;

	if (!windowFocused)
	{
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
#pragma endregion

#pragma region Static Callback

void Program::OnWindowFocus(GLFWwindow* window, int focused)
{
	Program* instance = Program::instance;
	if (instance)
		instance->WindowFocus(focused);

	if (focused)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else 
	{
		
	}

	
}

void Program::OnWindowResize(GLFWwindow* window, int width, int height)
{
	Program* instance = Program::instance;
	if (instance)
		instance->WindowResize(Vector2Int(width, height));
}

void Program::OnMouseMove(GLFWwindow* window, double xPos, double yPos)
{
	Program* instance = Program::instance;
	if (instance)
	{
		instance->MouseMove(Vector2(xPos, yPos));
	}
}

void Program::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	Program* instance = Program::instance;
	if (instance)
	{
		if (action == GLFW_PRESS)
		{
			instance->MousePressed(button);
		}
		else if (action == GLFW_RELEASE)
		{
			instance->MouseReleased(button);
		}
	}
}

void Program::OnMouseWheel(GLFWwindow* window, double xDelta, double yDelta)
{
	Program* instance = Program::instance;
	if (instance)
	{
		instance->MouseScroll(yDelta);
	}
}

void Program::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Program* instance = Program::instance;
	if (instance)
	{
		if (action == GLFW_PRESS)
		{
			instance->KeyPressed(key);
		}
		else if (action == GLFW_RELEASE)
		{
			instance->KeyReleased(key);
		}
	}
}

#pragma endregion