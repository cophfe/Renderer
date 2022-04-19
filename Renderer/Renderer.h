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
#include "FrameBuffer.h"
#include "MeshPrimitive.h"

class Renderer
{
public:
	void Init(const char* texturePath);
	void Cleanup();

	void UpdateUniformBuffers();
	void Render();
	void OnWindowResize(Vector2Int newSize);

	//set whether materials, shaders or meshes initiated with InitNew are automatically registered for rendering or not
	void SetAutoRegister(bool materials, bool shaders, bool meshes);
	bool GetAutoRegisterMaterials() { return autoRegisterMat; }
	bool GetAutoRegisterShaders() { return autoRegisterShad; }
	bool GetAutoRegisterMeshes() { return autoRegisterMesh; }
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

	Vector4 ambientColour = 0.08f * Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	void SetPBRValues(Material* pbrMaterial, float roughnessMultiplier = 0.5f, const char* albedo = nullptr, 
		const char* normal = nullptr, const char* roughness = nullptr, const char* metallic = nullptr, 
		const char* ao = nullptr, bool defaultMetallic = false, bool setIrradianceMap = true);
private:
	GLFWwindow* window;

	TextureManager textureManager;
	bool autoRegisterMat, autoRegisterShad, autoRegisterMesh;
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

	//these ones are stored seperately because I don't want these to be accessed outside of this class
	FrameBuffer frameBuffer;
	Material frameMaterial;
	Mesh frameQuad;
	Material skyboxMaterial;
	Texture2D irradienceMap;
	Texture2D environmentMap;

	struct CameraBufferStruct {
		Matrix4x4 viewProjectionMatrix;
		Matrix4x4 projectionMatrix;
		Matrix4x4 viewMatrix;
		Matrix4x4 inverseViewMatrix;
		Vector3 cameraPosition;
		float aspect;
		float fov;
	};

	struct TimingBufferStruct {
		float time;
		float deltaTime;
	};

	struct LightingBufferStruct {
		LightDataStruct lights[8];
		Vector3 ambientColour;
	};

	//stored uniform buffers
	LightingBufferStruct lightingBufferData;
};

