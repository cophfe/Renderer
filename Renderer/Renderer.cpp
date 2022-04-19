#include "Renderer.h"
#include "Program.h"

void Renderer::Init(const char* texturePath)
{
	if (!glfwInit())
	{
		throw "GLFW failed to initiate.";
		return;
	}
	
	const Vector2Int windowSize = Vector2Int(1280, 720);
	window = glfwCreateWindow(windowSize.x, windowSize.y, "yo", nullptr, nullptr);

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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); //needs to be LEQUAL for skybox to work
	glClearColor(0.09f, 0.1f, 0.1f, 1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	textureManager.Init(texturePath, 0, TextureFiltering::Linear, TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode::Wrap);
	SetAutoRegister(true, true, true);

	//init uniform buffers
	cameraBuffer.Init(sizeof(CameraBufferStruct), true, 0);
	timingBuffer.Init(sizeof(TimingBufferStruct), true, 1);
	lightingBuffer.Init(sizeof(LightingBufferStruct), true, 2);

	//init framebuffer
	frameBuffer.Init(windowSize, GL_RGBA16F, GL_DEPTH_COMPONENT);
	//vertex shader used to draw quad used for rendering screen texture to screen
	//i am only doing this as text because I wanted to actually use the initraw function somewhere
	Shader* basicVertex = Shader::InitNewRaw(
						"#version 460 core\n\
						layout (location = 4) in vec2 texCoord;\n\
						out vec2 TexCoord;\n\
						void main()\n\
						{ gl_Position = vec4(texCoord,1.0, 1.0);\n\
						TexCoord = texCoord * 0.5 + vec2(0.5); }", Shader::Type::Vertex);
	Shader* hdrFragment = Shader::InitNew("Shaders/ToneMapping.frag", Shader::Type::Fragment);
	frameMaterial.Init(*basicVertex, *hdrFragment);
	frameMaterial.SetTextureSampler("HDR", frameBuffer.GetColour());
	//init framebuffer quad
	Vector2 clipSpacePos[4] = {Vector2(-1,-1), Vector2(1, -1), Vector2(1, 1), Vector2(-1, 1)};
	GLuint order[6] = {0, 1, 2, 0, 2, 3};
	MeshData quad;
	//mesh with only texcoord
	//this mesh is used for both the sky 'box' and the screen rendering
	quad.AllocateMeshData(4, 6, MESH_TEXCOORDS);
	quad.SetTexCoords(clipSpacePos, 4);
	quad.SetIndices(order, 6);
	frameQuad.Init(&quad, 1);
	glEnableVertexAttribArray(0);
	
	//now setup skybox
	auto* skyboxVert = Shader::InitNew("Shaders/skybox.vert", Shader::Type::Vertex);
	skyboxMaterial.Init(*skyboxVert, *Shader::InitNew("Shaders/skybox.frag", Shader::Type::Fragment));
	Texture2D* skymap = textureManager.LoadTextureParams("skymap.jpg", 0, TextureFiltering::Linear, TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode::MirrorWrap, GL_RGBA16F);
	skyboxMaterial.SetTextureSampler("_SkyMap", skymap);

	//real quick make irradiance map
	//using local variables so that they will be deleted at the end
	irradienceMap.InitEmpty(Vector2Int(32, 32), GL_RGBA16F, 0, TextureFiltering::Linear);
	Material irradienceMaterial;
	Shader irrFrag;
	irrFrag.Init("Shaders/irradience.frag", Shader::Type::Fragment);
	irradienceMaterial.Init(*skyboxVert, irrFrag);
	irradienceMaterial.SetTextureSampler("_SkyMap", skymap);

	//note: using a framebuffer here is super slow
	glViewport(0, 0, irradienceMap.GetSize().x, irradienceMap.GetSize().y);
	FrameBuffer localFrameBuffer;
	localFrameBuffer.Init(&irradienceMap, nullptr);
	localFrameBuffer.Use();
	{
		irradienceMaterial.Use();
		frameQuad.Render();
	}
	localFrameBuffer.ClearUsed();
	glGenerateTextureMipmap(irradienceMap.GetID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)TextureFiltering::Linear);
	glViewport(0, 0, windowSize.x, windowSize.y);
	//(hopefully) successfully created irradience map!!! any material that sets its pbr values will have the irradience map set as well

	//and now make environment map
	//once again, so, so cruel to the computer to use framebuffers like this
	//i apologise to everyone with eyes for my sins
	/*environmentMap.InitEmpty(skymap->GetSize(), GL_RGBA16F, 5);
	localFrameBuffer = FrameBuffer();
	localFrameBuffer.Init(&environmentMap, nullptr);
	Material envMat;
	Shader envFrag;
	envFrag.Init("Shaders/environment.frag", Shader::Type::Fragment);
	envMat.Init(envFrag, *skyboxVert);
	localFrameBuffer.Use();
	{
		for (int i = 0; i < 5; i++)
		{

		}
		pow(0.5f, 0.5f)
		envMat.Use();
		frameQuad.Render();
	}
	localFrameBuffer.ClearUsed();*/
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

void Renderer::UpdateUniformBuffers()
{
	if (cameras.size() > 0)
	{
		CameraComponent* mainCamera = cameras[0];
		mainCamera->UpdateCamera();
		CameraBufferStruct cameraData;
		cameraData.projectionMatrix = mainCamera->GetProjectionMatrix();
		cameraData.viewMatrix = mainCamera->GetViewMatrix();
		cameraData.viewProjectionMatrix = mainCamera->GetViewProjectionMatrix();
		cameraData.inverseViewMatrix = glm::inverse(mainCamera->GetViewMatrix());
		cameraData.cameraPosition = mainCamera->GetTransform().GetLocalPosition();
		cameraData.aspect = mainCamera->GetAspect();
		cameraData.fov = mainCamera->GetFOV();
		
		cameraBuffer.BufferSubData(0, sizeof(cameraData), &cameraData);
	}

	for (size_t i = 0; i < lights.size() && i < 8; i++)
	{
		lightingBufferData.lights[i].direction = lights[i]->GetTransform().GetForward();
		lightingBufferData.lights[i].position = lights[i]->GetTransform().GetPosition();
	}
	lightingBuffer.BufferSubData(0, sizeof(lightingBufferData), &lightingBufferData);

	TimingBufferStruct time;
	
	time.time = glfwGetTime();
	time.deltaTime = Program::GetInstance()->GetDeltaTime();

	timingBuffer.BufferSubData(0, sizeof(time), &time);
}

void Renderer::Render()
{
	if (cameras.size() <= 0)
		return;

	//render everything to screen texture
	frameBuffer.Use();
	{
		glDepthFunc(GL_LEQUAL);
		glClear(GL_DEPTH_BUFFER_BIT);
		cameras[0]->UpdateCamera();
		for (MeshRendererComponent* meshRenderer : renderers)
		{
			meshRenderer->Render();
		}
		//now draw skybox
		skyboxMaterial.Use();
		frameQuad.Render();
	}
	frameBuffer.ClearUsed();

	glDepthFunc(GL_ALWAYS);
	//render screen texture to screen
	frameMaterial.Use();
	frameQuad.Render();

	glfwSwapBuffers(window);
}

void Renderer::OnWindowResize(Vector2Int newSize)
{
	if (GetMainCamera())
		GetMainCamera()->SetAspect(newSize);
	
	glViewport(0, 0, newSize.x, newSize.y);

	//resizing the framebuffer just made the screen go black, and I'm lazy, so here's a workaround
	frameBuffer = FrameBuffer();
	frameBuffer.Init(newSize, GL_RGBA16F, GL_DEPTH_COMPONENT);
	frameMaterial.SetTextureSampler("HDR", frameBuffer.GetColour());
}

void Renderer::SetAutoRegister(bool materials, bool shaders, bool meshes)
{
	autoRegisterMat = materials;
	autoRegisterShad = shaders;
	autoRegisterMesh = meshes;
}

Material* Renderer::RegisterMaterial(Material* mat)
{
	if (!autoRegisterMat)
		return nullptr;

	materials.push_back(mat);
	return mat;
}

Shader* Renderer::RegisterShader(Shader* shader)
{
	if (!autoRegisterShad)
		return nullptr;

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

void Renderer::SetPBRValues(Material* pbrMaterial, float roughnessMultiplier, const char* albedo, const char* normal, const char* roughness, const char* metallic, const char* ao, bool defaultMetallic, bool setIrradianceMap)
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

	if (setIrradianceMap)
		pbrMaterial->SetTextureSampler("_IrradienceMap", &irradienceMap);
	else
		pbrMaterial->SetTextureSampler("_IrradienceMap", nullptr);

	pbrMaterial->SetUniform("_Material.roughness", roughnessMultiplier);
}

