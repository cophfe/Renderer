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

Material* Program::CreateMaterial(Shader* vertex, Shader* fragment)
{
	Material* material = Material::InitNew(*vertex, *fragment);
	materials.push_back(material);
	return material;
}

Shader* Program::CreateShader(const char* path, Shader::Type type)
{
	Shader* shader = Shader::InitNew(path, type);
	shaders.push_back(shader);
	return shader;
}

Mesh* Program::CreateMesh(MeshData& data, bool isStatic, bool storeMeshOnCPU)
{
	Mesh* mesh = Mesh::InitNew(data, isStatic, storeMeshOnCPU);
	meshes.push_back(mesh);
	return mesh;
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

	Vector2Int size;
	glfwGetWindowSize(window, &size.x, &size.y);
	camera = new Camera(glm::radians(65.0f), size.x / (float)size.y, 0.001, 100);

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	lastMousePosition = Vector2(x, y);

	lastTime = glfwGetTime();
	shaderRecompileTimer = shaderRecompileTime;

	Shader* fragment = CreateShader("Default.fsd", Shader::Type::Fragment);
	Shader* vertex = CreateShader("Default.vsd", Shader::Type::Vertex);
	Material* defaultMaterial = CreateMaterial(vertex, fragment);

	MeshData data;
	MeshPrimitive::SetCube(data);
	memset(data.colours, 0xFF, data.vertexCount * sizeof(MeshData::Colour));

	Mesh* mesh = CreateMesh(data);
	auto& t = CreateObject(mesh, defaultMaterial)->GetTransform();
	t.SetPosition(Vector3(0, 0, -1));
	
	//t = CreateObject(mesh, defaultMaterial)->GetTransform();
	//t.SetPosition(Vector3(-5, 0, -4));
	
	//auto& t = objects[0]->GetTransform();
	//t.SetPosition(Vector3(0, 0.5f, 0));
	
	/*Vector3 vertices[] = {
		Vector3(0.5f,0,0),
		Vector3(-0.5f, 0,0),
		Vector3(0,0.5f,0),
		Vector3(-0.5f, 0,0),
		Vector3(0.5f,0,0),
		Vector3(0,-0.5f,0),
	};
	MeshData data2;
	const int vertCount = 6;
	data2.AllocateMeshData(vertCount, 0);
	data2.SetPositions(vertices, vertCount);
	data2.SetColours(colours, vertCount);
	Mesh* mesh2 = Mesh::InitNew(data2);
	objects.push_back(new Object(mesh2, defaultMaterial));*/

}

void Program::InitGraphics()
{
	if (!glfwInit())
	{
		throw "GLFW failed to initiate.";
		return;
	}

	window = glfwCreateWindow(1280, 720, "yo", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		throw "GLFW failed to create window.";
		return;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		throw "GLAD failed to load.";
		return;
	}

	//cap fps to framerate
	glfwSwapInterval(1);

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(OpenGLDebugCallback, 0); //callback has never actually called, should probably use glfwSetErrorCallback or something idk
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.09f, 0.1f, 0.1f, 1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Program::InitCallbacks()
{
	glfwSetWindowSizeCallback(window, OnWindowResize);
	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetScrollCallback(window, OnMouseWheel);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetWindowFocusCallback(window, OnWindowFocus);
}

void Program::Loop()
{
	while (!glfwWindowShouldClose(window))
	{
		UpdateTime();
		Render();
		glfwPollEvents();
		Update();
	}
}

void Program::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->UpdateCamera();
	glUniformMatrix4fv(materials[0]->GetUniformID("ViewProjectionMatrix"), 1, GL_FALSE, (GLfloat*)&(camera->GetViewProjectionMatrix()[0]));
	
	size_t size = objects.size();
	for (size_t i = 0; i < size; i++)
	{
		objects[i]->Render();
	} 

	glfwSwapBuffers(window);
}

void Program::UpdateInput()
{

}

void Program::Update()
{
	size_t size = objects.size();
	Quaternion rotation = glm::angleAxis(glm::radians(200.0f) * (float)deltaTime, glm::normalize(Vector3(1, 1, 1)));
	for (size_t i = 0; i < size; i++)
	{
		//objects[i]->GetTransform().Rotate(rotation);
	}


	Vector3 cameraDir = Vector3();
	Transform& t = camera->GetTransform();
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

void Program::Cleanup()
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	materials.clear();

	for (size_t i = 0; i < meshes.size(); i++)
	{
		delete meshes[i];
	}
	meshes.clear();

	for (size_t i = 0; i < materials.size(); i++)
	{
		delete materials[i];
	}
	materials.clear();

	for (size_t i = 0; i < shaders.size(); i++)
	{
		delete shaders[i];
	}
	shaders.clear();

	glfwTerminate();
}

void Program::RecompileShaders()
{
	size_t size = shaders.size();
	for (size_t i = 0; i < size; i++)
	{
		if (shaders[i]->ShouldRecompile())
		{
			shaders[i]->Recompile();

			for (size_t i = 0; i < materials.size(); i++)
			{
				if (materials[i]->UsesShader(shaders[i]))
				{
					materials[i]->ReloadMaterial();
				}
			}
		}
	}
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void Program::MouseReleased(int button)
{
}

void Program::MouseMove(Vector2 delta)
{
	auto& t = camera->GetTransform();

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

//https://www.khronos.org/opengl/wiki/Example/OpenGL_Error_Testing_with_Message_Callbacks
void Program::OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

#pragma endregion