#include "Program.h"
#include "MeshPrimitive.h"

void Program::Run()
{
	Init();
	Loop();
	Cleanup();
}

void Program::Init()
{
	InitGraphics();

	Shader* fragment = Shader::InitNew("Default.fsd", Shader::Type::Fragment);
	Shader* vertex = Shader::InitNew("Default.vsd", Shader::Type::Vertex);
	shaders.push_back(fragment);
	shaders.push_back(vertex);

	Material* defaultMaterial = Material::InitNew(*vertex, *fragment);

	Vector4 colours[] = {
		Vector4(1,0,0,1),
		Vector4(0,1,0,1),
		Vector4(0,0,1,1),
		Vector4(1,1,1,1),
		Vector4(0,1,1,1),
		Vector4(1,0,1,1),
		Vector4(1,1,0,1),
		Vector4(0,0,0,1),
	};

	MeshData data;
	MeshPrimitive::SetCube(data);
	
	data.SetColours(colours, 8);
	Mesh* mesh = Mesh::InitNew(data);

	objects.push_back(new Object(mesh, defaultMaterial));
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Program::Loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		size_t size = objects.size();

		Quaternion rotation = glm::angleAxis(glm::radians(1.0f), glm::normalize(Vector3(1, 1, 1)));
		for (size_t i = 0; i < size; i++)
		{
			objects[i]->Render();
			Quaternion rot = objects[i]->GetTransform().GetRotation();
			objects[i]->GetTransform().SetRotation(rot * rotation);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Program::Cleanup()
{
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
