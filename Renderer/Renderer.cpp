#include "Renderer.h"
#include "Program.h"

void Renderer::Init()
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

	Vector2Int size;
	glfwGetWindowSize(window, &size.x, &size.y);
	mainCamera = new Camera(glm::radians(65.0f), size.x / (float)size.y, 0.001, 100);
}

void Renderer::Cleanup()
{
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

void Renderer::RecompileShaders()
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

void Renderer::Render(Object** objects, size_t count)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mainCamera->UpdateCamera();
	//glUniformMatrix4fv(materials[0]->GetUniformID("ViewProjectionMatrix"), 1, GL_FALSE, (GLfloat*)&(camera->GetViewProjectionMatrix()[0]));

	for (size_t i = 0; i < count; i++)
	{
		objects[i]->Render();
	}

	glfwSwapBuffers(window);
}

Material* Renderer::CreateMaterial(Shader* vertex, Shader* fragment)
{
	Material* material = Material::InitNew(*vertex, *fragment);
	materials.push_back(material);
	return material;
}

Shader* Renderer::CreateShader(const char* path, Shader::Type type)
{
	Shader* shader = Shader::InitNew(path, type);
	shaders.push_back(shader);
	return shader;
}

Mesh* Renderer::CreateMesh(MeshData& data, bool isStatic, bool storeMeshOnCPU)
{
	Mesh* mesh = Mesh::InitNew(data, isStatic, storeMeshOnCPU);
	meshes.push_back(mesh);
	return mesh;
}
