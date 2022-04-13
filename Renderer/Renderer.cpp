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

	renderers.clear();
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
	if (cameras.size() > 0)
	{
		CameraComponent* mainCamera = cameras[0];
		mainCamera->UpdateCamera();
		CameraBufferStruct cameraData;
		cameraData.ProjectionMatrix = mainCamera->GetProjectionMatrix();
		cameraData.ViewMatrix = mainCamera->GetViewMatrix();
		cameraData.ViewProjectionMatrix = mainCamera->GetViewProjectionMatrix();
		cameraData.InverseViewMatrix = glm::inverse(mainCamera->GetViewMatrix());
		cameraData.cameraPosition = Vector4(mainCamera->GetTransform().GetLocalPosition(), 1.0f);

		cameraBuffer.BufferSubData(0, sizeof(cameraData), &cameraData);
	}

	for (size_t i = 0; i < lights.size() && i < 8; i++)
	{
		lightingBufferData.lights[i].direction = lights[i]->GetTransform().GetForward();
		lightingBufferData.lights[i].position = lights[i]->GetTransform().GetPosition();
	}
	lightingBuffer.BufferSubData(0, sizeof(lightingBufferData), &lightingBufferData);

	TimingBufferStruct time;
	
	time.Time = glfwGetTime();
	time.DeltaTime = Program::GetInstance()->GetDeltaTime();

	timingBuffer.BufferSubData(0, sizeof(time), &time);
}

void Renderer::Render()
{
	if (cameras.size() <= 0)
		return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cameras[0]->UpdateCamera();

	for (MeshRendererComponent* meshRenderer : renderers)
	{
		meshRenderer->Render();
	}

	glfwSwapBuffers(window);
}

Material* Renderer::RegisterMaterial(Material* mat)
{
	materials.push_back(mat);
	return mat;
}

Shader* Renderer::RegisterShader(Shader* shader)
{
	shaders.push_back(shader); 
	return shader;
}

Mesh* Renderer::RegisterMesh(Mesh* mesh)
{
	meshes.push_back(mesh); 
	return mesh;
}

void Renderer::RegisterRenderer(MeshRendererComponent* renderer)
{
	renderers.push_back(renderer);
}

void Renderer::DeregisterRenderer(MeshRendererComponent* renderer)
{
	std::vector<MeshRendererComponent*>::iterator position = std::find(renderers.begin(), renderers.end(), renderer);
	if (position != renderers.end())
		renderers.erase(position);
}

void Renderer::RegisterLight(LightComponent* light)
{
	lights.push_back(light);

	UpdateLights();
}

void Renderer::DeregisterLight(LightComponent* light)
{
	std::vector<LightComponent*>::iterator position = std::find(lights.begin(), lights.end(), light);
	if (position != lights.end())
		lights.erase(position);
	
	UpdateLights();
}

void Renderer::RegisterCamera(CameraComponent* camera)
{
	cameras.push_back(camera);
	
	FindMainCamera();
}

void Renderer::DeregisterCamera(CameraComponent* camera)
{
	std::vector<CameraComponent*>::iterator position = std::find(cameras.begin(), cameras.end(), camera);
	if (position != cameras.end())
		cameras.erase(position);
	
	FindMainCamera();
}

void Renderer::UpdateLights()
{
	//update light buffer
	for (size_t i = 0; i < lights.size() && i < 8; i++)
	{
		lightingBufferData.lights[i] = lights[i]->GetLightData();
	}
	
	//if (lights.size() < 8)
	//{
	//	memset(lightingBufferData.lights + lights.size() - 1, 0, sizeof(LightDataStruct) * 8 - lights.size());
	//}

	lightingBufferData.ambientColour = ambientColour;
	lightingBuffer.BufferSubData(0, sizeof(lightingBufferData), &lightingBufferData);
}

void Renderer::FindMainCamera()
{
	if (cameras.size() == 0)
		return;

	int priority = cameras[0]->GetPriority();

	for (size_t i = 1; i < cameras.size(); i++)
	{
		int newP = cameras[i]->GetPriority();
		if (newP > priority)
		{
			priority = newP;
			
			CameraComponent* cache = cameras[0];
			cameras[0] = cameras[i];
			cameras[i] = cache;
		} 
	}
}

void Renderer::SetPBRValues(Material* pbrMaterial, float roughnessMultiplier, const char* albedo, const char* normal, const char* roughness, const char* metallic, const char* ao, bool defaultMetallic)
{
	if (!pbrMaterial)
		return;

	if (albedo)
		pbrMaterial->SetTextureSampler("_AlbedoMap", textureManager.LoadTexture(albedo));
	else
		pbrMaterial->SetTextureSampler("_AlbedoMap", textureManager.LoadTexture("white.png"));

	if (normal)
		pbrMaterial->SetTextureSampler("_NormalMap", textureManager.LoadTexture(normal));
	else
		pbrMaterial->SetTextureSampler("_NormalMap", textureManager.LoadTexture("normal.png"));

	if (roughness)
		pbrMaterial->SetTextureSampler("_RoughnessMap", textureManager.LoadTexture(roughness));
	else
		pbrMaterial->SetTextureSampler("_RoughnessMap", textureManager.LoadTexture("white.png"));

	if (metallic)
		pbrMaterial->SetTextureSampler("_MetallicMap", textureManager.LoadTexture(metallic));
	else
	{
		if (defaultMetallic)
			pbrMaterial->SetTextureSampler("_MetallicMap", textureManager.LoadTexture("white.png"));
		else
			pbrMaterial->SetTextureSampler("_MetallicMap", nullptr);
	}

	if (ao)
		pbrMaterial->SetTextureSampler("_AmbientOcclusionMap", textureManager.LoadTexture(ao));
	else
		pbrMaterial->SetTextureSampler("_AmbientOcclusionMap", textureManager.LoadTexture("white.png"));

	pbrMaterial->SetUniform("_Material.roughness", roughnessMultiplier);
}

