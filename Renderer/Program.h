#pragma once
#include "Graphics.h"
#include <vector>
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "UniformBuffer.h"
#include "Renderer.h"
#include "MeshBuilder.h"
#include "GameObject.h"

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

	void RegisterGameObject(GameObject* gameObject);
	void DeregisterGameObject(GameObject* gameObject);
	void AddChild(Transform& child);
	void RemoveChild(Transform& child);

	void SetCursorLocked(bool cursorLocked);
	inline Vector2 GetCursorPos() const { return cursorPosition; }
	inline bool GetWindowFocused() const { return windowFocused; }
private:
	Program() = default;
	Program(const Program& other) = delete;
	Program& operator=(const Program& other) = delete;

	static Program* instance;

	void Init();
	void Start();
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
	void WindowFocus(bool focus);
#pragma endregion

	Renderer renderer;
	
	std::vector<GameObject*> gameObjects = std::vector<GameObject*>();
	std::vector<Transform*> children = std::vector<Transform*>();

	std::vector<GameObject*> newGameObjects = std::vector<GameObject*>();
	std::vector<Component*> newComponents = std::vector<Component*>(); //should be used when a game object has been created and new components have been added to it later, after start() has already been called on the gameobject
	double deltaTime;
	double lastTime;
	bool windowFocused = true;
	bool initiated;

	//light stuff
	std::vector<LightComponent*> pointLights;
	Mesh* lightMesh;
	Shader* lightVert;
	Shader* lightFrag;
	int lightIndex = 0;

	const float shaderRecompileTime = 1;
	float shaderRecompileTimer;
	Vector2 cursorPosition;
};

