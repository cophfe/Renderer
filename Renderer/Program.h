#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Object.h"
#include "Camera.h"

class Program
{
public:
	static Program* CreateInstance();
	static void DestroyInstance();
	inline static Program* GetInstance() { return instance; }

	void Run();
	inline float GetDeltaTime() { return deltaTime; }
	inline double GetDeltaTimeDouble() { return deltaTime; }
	Material* CreateMaterial(Shader* vertex, Shader* fragment);
	Shader* CreateShader(const char* path, Shader::Type type);
	Mesh* CreateMesh(MeshData& data, bool isStatic = true, bool storeMeshOnCPU = false);
	Object* CreateObject(Mesh* mesh, Material* material);

private:
	Program() = default;
	Program(const Program& other) = delete;
	Program& operator=(const Program& other) = delete;

	static Program* instance;

	void Init();
	void InitGraphics();
	void InitCallbacks();
	void Loop();
	void Render();
	void UpdateInput();
	void Update();
	void Cleanup();
	void RecompileShaders();
	void UpdateTime();

#pragma region Callbacks
	static void OnWindowFocus(GLFWwindow* window, int focused);
	static void OnWindowResize(GLFWwindow* window, int width, int height);
	static void OnMouseMove(GLFWwindow* window, double xPos, double yPos);
	static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void OnMouseWheel(GLFWwindow* window, double xDelta, double yDelta);
	static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);

	void KeyPressed(int key);
	void KeyReleased(int key);
	void MousePressed(int button);
	void MouseReleased(int button);
	void MouseMove(Vector2 delta);
	void MouseScroll(float delta);
	void WindowResize(Vector2Int size);
#pragma endregion


	static void OpenGLDebugCallback(GLenum source,
		GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	GLFWwindow* window;

	std::vector<Shader*> shaders;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	std::vector<Object*> objects;
	Camera* camera;

	double deltaTime;
	double lastTime;
	//to be implemented once I get a timer system
	const float shaderRecompileTime = 1;
	float shaderRecompileTimer;
	Vector2 lastMousePosition;
};

