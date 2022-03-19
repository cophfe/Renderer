#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Object.h"

class Program
{
public:
	void Run();

private:
	void Init();
	void InitGraphics();
	void Loop();
	void Cleanup();
	void RecompileShaders();

	static void OpenGLDebugCallback(GLenum source,
		GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	GLFWwindow* window;

	std::vector<Shader*> shaders;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	std::vector<Object*> objects;

	//to be implemented once I get a timer system
	const float shaderRecompileTime = 1;
	float shaderRecompileTimer;
};

