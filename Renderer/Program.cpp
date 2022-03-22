#include "Program.h"
#include "MeshPrimitive.h"

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

Object* Program::CreateObject(Mesh* mesh, Material* material)
{
	Object* object = new Object(mesh, material);
	objects.push_back(object);
	return object;
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

	Shader* fragment = renderer.CreateShader("Default.fsd", Shader::Type::Fragment);
	Shader* vertex = renderer.CreateShader("Default.vsd", Shader::Type::Vertex);
	Material* defaultMaterial = renderer.CreateMaterial(vertex, fragment);

	MeshData data;
	MeshPrimitive::SetCube(data);
	memset(data.colours, 0xFF, data.vertexCount * sizeof(MeshData::Colour));

	Mesh* mesh = renderer.CreateMesh(data);
	auto& t = CreateObject(mesh, defaultMaterial)->GetTransform();
	t.SetPosition(Vector3(0, 0, -1));
	
	t = CreateObject(mesh, defaultMaterial)->GetTransform();
	t.SetPosition(Vector3(-5, 0, -4));
	
}

void Program::InitGraphics()
{
	renderer.Init();
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
		UpdateInput();
	}
}

void Program::Render()
{
	renderer.Render(objects.data(), objects.size());
}

void Program::UpdateInput()
{
	Vector3 cameraDir = Vector3();
	Transform& t = renderer.GetMainCamera()->GetTransform();

	GLFWwindow* window = GetWindow();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraDir -= t.GetForward();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraDir += t.GetForward();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraDir -= t.GetRight();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraDir += t.GetRight();
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cameraDir -= t.GetUp();
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		cameraDir += t.GetUp();
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		cameraDir *= 2;
	t.Move(cameraDir * ((float)deltaTime * 2.0f));
}

void Program::Update()
{
	size_t size = objects.size();
	Quaternion rotation = glm::angleAxis(glm::radians(200.0f) * (float)deltaTime, glm::normalize(Vector3(1, 1, 1)));
	for (size_t i = 0; i < size; i++)
	{
		objects[i]->GetTransform().Rotate(rotation);
	}
}

void Program::Cleanup()
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();

	renderer.Cleanup();
}

void Program::RecompileShaders()
{
	renderer.RecompileShaders();
}

void Program::UpdateTime()
{
	double newTime = glfwGetTime();
	deltaTime = newTime - lastTime;
	lastTime = newTime;

	shaderRecompileTimer -= deltaTime;
	if (shaderRecompileTimer < 0)
	{
		RecompileShaders();
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
	glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void Program::MouseReleased(int button)
{
}

void Program::MouseMove(Vector2 delta)
{
	auto& t = renderer.GetMainCamera()->GetTransform();

	static Vector2 rotation = Vector2();
	rotation += delta * -0.0007f;
	rotation.y = glm::clamp(rotation.y, glm::radians(- 90.0f) , glm::radians(90.0f));
	t.SetRotation(glm::angleAxis(rotation.x, Vector3(0,1,0)) * glm::angleAxis(rotation.y, Vector3(1, 0, 0)));
}

void Program::MouseScroll(float delta)
{

}

void Program::WindowResize(Vector2Int size)
{
	//if (camera)
	//	camera->SetAspect(size);
}
#pragma endregion

#pragma region Static Callback

void Program::OnWindowFocus(GLFWwindow* window, int focused)
{
	if (focused)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else 
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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