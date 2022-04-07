#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Object.h"
#include "TextureManager.h"
#include "UniformBuffer.h"
#include "MeshRendererComponent.h"

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

	inline Camera* GetMainCamera() { return mainCamera; }
	inline GLFWwindow* GetWindow() const { return window; }
	inline TextureManager& GetTextureManager() { return textureManager; }
	inline std::vector<Material*>& GetMaterials() { return materials; }

	Renderer() = default;
	Renderer(const Renderer& other) = delete;
	Renderer& operator= (const Renderer& other) = delete;

	Vector4 ambientColour = Vector4(0.05f, 0.05f, 0.05f, 1.0f);

private:
	GLFWwindow* window;

	const float shaderRecompileTime = 1;
	float shaderRecompileTimer;
	TextureManager textureManager;
	std::vector<Shader*> shaders;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	std::vector<MeshRendererComponent*> renderers;

	Camera* mainCamera;

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

	struct LightDataStruct //aligned to 16
	{
		Vector3 position;
		int : 32;
		Vector3 luminance;
		int : 32;
		Vector3 direction;
		float linear;
		float quadratic;
		float minAngle;
		float maxAngle;
		int type = 0;
		int : 32;
		int : 32;
		int : 32;
		int : 32;
	};
	struct LightingBufferStruct {
		LightDataStruct lights[8];

		Vector4 ambientColour;
	};
};

