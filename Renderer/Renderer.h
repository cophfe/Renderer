#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Object.h"
#include "UniformBuffer.h"

class Renderer
{
public:
	void Init();
	void Cleanup();
	void RecompileShaders();

	void Render(Object** objects, size_t count);
	Material* CreateMaterial(Shader* vertex, Shader* fragment);
	Shader* CreateShader(const char* path, Shader::Type type);
	Mesh* CreateMesh(MeshData& data, bool isStatic = true, bool storeMeshOnCPU = false);

	inline Camera* GetMainCamera() { return mainCamera; }
	inline GLFWwindow* GetWindow() const { return window; }

private:
	GLFWwindow* window;

	const float shaderRecompileTime = 1;
	float shaderRecompileTimer;

	std::vector<Shader*> shaders;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	Camera* mainCamera;
};

