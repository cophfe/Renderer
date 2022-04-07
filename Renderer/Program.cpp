#include "Program.h"
#include "MeshPrimitive.h"
#include "MeshRendererComponent.h"

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

void Program::Init()
{
	InitGraphics();
	InitCallbacks();

	double x, y;
	glfwGetCursorPos(GetWindow(), &x, &y);
	lastMousePosition = Vector2(x, y);

	lastTime = glfwGetTime();
	shaderRecompileTimer = shaderRecompileTime;

	auto& tM = renderer.GetTextureManager();

	Shader* fragment = Shader::InitNew("Shaders/Default.frag", Shader::Type::Fragment);
	Shader* vertex = Shader::InitNew("Shaders/Default.vert", Shader::Type::Vertex);
	Material* defaultMaterial = Material::InitNew(*vertex, *fragment);
	defaultMaterial->SetTextureSampler("_DiffuseMap", tM.LoadTexture("soulspear_diffuse.tga"));
	defaultMaterial->SetTextureSampler("_NormalMap", tM.LoadTexture("soulspear_normal.tga"));

	MeshData data;
	MeshPrimitive::SetCube(data);

	Material* metalMaterial = Material::InitNew(*vertex, *fragment);
	metalMaterial->SetTextureSampler("_DiffuseMap", tM.LoadTexture("metal_diffuse.jpg"));
	metalMaterial->SetTextureSampler("_NormalMap", tM.LoadTexture("metal_normal.jpg"));
	Mesh* mesh = Mesh::InitNew(&data, 1);

	for (size_t i = 1; i < 30; i++)
	{
		auto gO = GameObject::Create();
		MeshRendererComponent* component = gO->AddComponent<MeshRendererComponent>();
		component->Init(mesh, metalMaterial);
		gO->GetTransform().SetLocalPosition(Vector3(4, 0, -(1.5 * i)));
	}
	
	//make soulspear
	int submeshCount;
	auto* datas = MeshBuilder::LoadMeshData(submeshCount, "Models/soulspear.obj");
	mesh = Mesh::InitNew(datas, submeshCount);
	MeshBuilder::FreeMeshArray(datas, submeshCount);

	Material* soulMaterial = Material::InitNew(*vertex, *fragment);
	soulMaterial->SetTextureSampler("_DiffuseMap", tM.LoadTexture("soulspear_diffuse.tga"));
	soulMaterial->SetTextureSampler("_NormalMap", tM.LoadTexture("soulspear_normal.tga"));
	soulMaterial->SetTextureSampler("_SpecularMap", tM.LoadTexture("soulspear_specular.tga"));

	auto soulSpear = GameObject::Create();
	MeshRendererComponent* soulSpearRenderer = soulSpear->AddComponent<MeshRendererComponent>();
	soulSpearRenderer->Init(mesh, soulMaterial);
	soulSpear->GetTransform().SetLocalPosition(Vector3(-2, -3, -10));


	

	


	//TEMP - SET MATERIAL INFORMATION
	auto& materials = renderer.GetMaterials();
	for (size_t i = 0; i < materials.size(); i++)
	{
		materials[i]->SetUniform("_Material.shininess", 32.0f);
		materials[i]->SetUniform("_Material.specularity", 0.6f);
	}
	//
}

void Program::InitGraphics()
{
	renderer.Init("Textures/");
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
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		UpdateTime();
		Render();
		glfwPollEvents();
		Update();

		if (cameraMoving)
		{
			Vector3 cameraDir = Vector3();
			Transform& t = renderer.GetMainCamera()->GetTransform();

			GLFWwindow* window = GetWindow();
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				cameraDir -= t.GetLocalForward();
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				cameraDir += t.GetLocalForward();
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				cameraDir -= t.GetLocalRight();
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				cameraDir += t.GetLocalRight();
			}
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			{
				cameraDir -= t.GetLocalUp();
			}
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			{
				cameraDir += t.GetLocalUp();
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
				cameraDir *= 2;
			t.LocalMove(cameraDir * ((float)deltaTime * 2.0f));
		}
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

	shaderRecompileTimer -= deltaTime;
	if (shaderRecompileTimer < 0)
	{
		renderer.RecompileShaders();
		shaderRecompileTimer = shaderRecompileTime;
	}
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
	if (button == GLFW_MOUSE_BUTTON_2)
	{
		cameraMoving = true;
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void Program::MouseReleased(int button)
{
	if (button == GLFW_MOUSE_BUTTON_2 && cameraMoving)
	{
		cameraMoving = false;
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Program::MouseMove(Vector2 delta)
{
	if (!windowFocused)
		return;

	if (cameraMoving)
	{
		auto& t = renderer.GetMainCamera()->GetTransform();

		static Vector2 rotation = Vector2();
		rotation += delta * -0.0007f;
		rotation.y = glm::clamp(rotation.y, glm::radians(-90.0f), glm::radians(90.0f));
		t.SetLocalRotation(glm::angleAxis(rotation.x, Vector3(0, 1, 0)) * glm::angleAxis(rotation.y, Vector3(1, 0, 0)));
	}
}

void Program::MouseScroll(float delta)
{

}

void Program::WindowResize(Vector2Int size)
{
	//if (camera)
	//	camera->SetAspect(size);
}
void Program::WindowFocus(bool focus)
{
	windowFocused = focus;

	if (!windowFocused && cameraMoving)
	{
		cameraMoving = false;
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
		Vector2 cursor = Vector2(xPos, yPos);
		instance->MouseMove(cursor - instance->lastMousePosition);
		instance->lastMousePosition = cursor;
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