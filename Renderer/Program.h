#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Object.h"
#include "Camera.h"
#include "UniformBuffer.h"
#include "Renderer.h"
#include "MeshBuilder.h"

class Program
{
public:
	static Program* CreateInstance();
	static void DestroyInstance();
	inline static Program* GetInstance() { return instance; }
	void Run();

	inline GLFWwindow* GetWindow() { return renderer.GetWindow(); }
	inline Renderer& GetRenderer() { return renderer; }
	inline float GetDeltaTime() { return deltaTime; }
	inline double GetDeltaTimeDouble() { return deltaTime; }
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
	void Update();
	void Cleanup();
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

	Renderer renderer;
	std::vector<Object*> objects;

	double deltaTime;
	double lastTime;

	const float shaderRecompileTime = 1;
	float shaderRecompileTimer;
	Vector2 lastMousePosition;
};

