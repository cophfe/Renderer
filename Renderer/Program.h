#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

class Program
{
public:
	void Run();

private:
	void Init();
	void Loop();
	void Cleanup();

	GLFWwindow* window;

	std::vector<Shader*> shaders;
	std::vector<Material*> materials;

};

