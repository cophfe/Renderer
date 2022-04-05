#include "Renderer.h"
#include "Program.h"

void Renderer::Init(const char* texturePath)
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

	Vector2Int size;
	glfwGetWindowSize(window, &size.x, &size.y);
	mainCamera = new Camera(glm::radians(65.0f), size.x / (float)size.y, 0.001, 100);

	textureManager.Init(texturePath, 0, TextureFiltering::Linear, TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode::Wrap);

	//init uniform buffers
	cameraBuffer.Init(sizeof(CameraBufferStruct), true, 0);
	timingBuffer.Init(sizeof(TimingBufferStruct), true, 1);
	lightingBuffer.Init(sizeof(LightingBufferStruct), true, 2);
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

	textureManager.Unload();
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

			//extremely major issue: shader uniforms get reset on recompile
			//also big issue: sometimes just dissappears
			for (size_t j = 0; j < materials.size(); j++)
			{
				if (materials[j]->UsesShader(shaders[i]))
				{
					materials[j]->ReloadMaterial();
				}
			}
		}
	}
}

void Renderer::UpdateUniformBuffers()
{
	CameraBufferStruct cameraData;
	cameraData.ProjectionMatrix = mainCamera->GetProjectionMatrix();
	cameraData.ViewMatrix = mainCamera->GetViewMatrix();
	cameraData.ViewProjectionMatrix = mainCamera->GetViewProjectionMatrix();
	cameraData.InverseViewMatrix = glm::inverse(mainCamera->GetViewMatrix());
	cameraData.cameraPosition = Vector4(mainCamera->GetTransform().GetLocalPosition(), 1.0f);
	cameraBuffer.BufferSubData(0, sizeof(cameraData), &cameraData);

	TimingBufferStruct time;
	time.Time = glfwGetTime();
	time.DeltaTime = Program::GetInstance()->GetDeltaTime();
	timingBuffer.BufferSubData(0, sizeof(time), &time);

	LightingBufferStruct lighting;
	lighting.ambientColour = ambientColour;

	//TEMP- SET FIRST LIGHTING THING TO A CAMERA LIGHT
	LightDataStruct light;
	light.position = mainCamera->GetTransform().GetLocalPosition();
	light.direction = mainCamera->GetTransform().GetLocalForward();
	light.luminance = Vector3(0.8f, 0.8f, 1.0f);
	light.linear = 0.2f;
	light.quadratic = 0.07f;
	light.minAngle = glm::cos(glm::radians(30.0f));
	light.maxAngle = glm::cos(glm::radians(40.0f));
	light.type = 2;
	lighting.lights[0] = light;
	
	memset(lighting.lights + 1, 0, sizeof(LightDataStruct) * 7);
	//std::cout << "x: " << light.direction.x << ", y: " << light.direction.y << ", z: " << light.direction.z << std::endl;
	//
	lightingBuffer.BufferSubData(0, sizeof(lighting), &lighting);
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

