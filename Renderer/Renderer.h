#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "TextureManager.h"
#include "UniformBuffer.h"
#include "MeshRendererComponent.h"
#include "LightComponent.h"
#include "CameraComponent.h"

class Renderer
{
public:
	void Init(const char* texturePath);
	void Cleanup();
	void RecompileShaders();

	void UpdateUniformBuffers();
	void Render();
	
	//note: these should definitely check if the things are already in the array
	//called by respective classes on init
	Material* RegisterMaterial(Material* mat);
	Shader* RegisterShader(Shader* shader);
	Mesh* RegisterMesh(Mesh* mesh);

	void RegisterRenderer(MeshRendererComponent* renderer);
	void DeregisterRenderer(MeshRendererComponent* renderer);
	
	void RegisterLight(LightComponent* light);
	void DeregisterLight(LightComponent* light);

	void RegisterCamera(CameraComponent* camera);
	void DeregisterCamera(CameraComponent* camera);

	void UpdateLights();
	void FindMainCamera();

	inline CameraComponent* GetMainCamera() const { return cameras.size() > 0 ? cameras[0] : nullptr; }
	inline GLFWwindow* GetWindow() const { return window; }
	inline TextureManager& GetTextureManager() { return textureManager; }
	inline std::vector<Material*>& GetMaterials() { return materials; }

	Renderer() = default;
	Renderer(const Renderer& other) = delete;
	Renderer& operator= (const Renderer& other) = delete;

	Vector4 ambientColour = Vector4(0.05f, 0.05f, 0.05f, 1.0f);

	void SetPBRValues(Material* pbrMaterial, float roughnessMultiplier = 0.5f, const char* albedo = nullptr, const char* normal = nullptr, const char* roughness = nullptr, const char* metallic = nullptr, const char* ao = nullptr, bool defaultMetallic = false);
private:
	GLFWwindow* window;

	const float shaderRecompileTime = 1;
	float shaderRecompileTimer;
	TextureManager textureManager;
	std::vector<Shader*> shaders;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

	//components used for rendering
	std::vector<MeshRendererComponent*> renderers;
	std::vector<LightComponent*> lights;
	std::vector<CameraComponent*> cameras;

	UniformBuffer timingBuffer;
	UniformBuffer cameraBuffer;
	UniformBuffer lightingBuffer;

	struct CameraBufferStruct {
		Matrix4x4 ViewProjectionMatrix;
		Matrix4x4 ProjectionMatrix;
		Matrix4x4 ViewMatrix;
		Matrix4x4 InverseViewMatrix;
		Vector4 cameraPosition;
	};

	struct TimingBufferStruct {
		float Time;
		float DeltaTime;
	};

	struct LightingBufferStruct {
		LightDataStruct lights[8];

		Vector3 ambientColour;
	};

	//uniform buffers (if stored)
	LightingBufferStruct lightingBufferData;
};

