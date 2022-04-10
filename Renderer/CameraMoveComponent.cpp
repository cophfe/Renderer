#include "CameraMoveComponent.h"
#include "Program.h"

void CameraMoveComponent::Init(float moveSpeed, float turnSpeed, float speedMod)
{
	this->moveSpeed = moveSpeed;
	this->turnSpeed = turnSpeed;
	this->speedMod = speedMod;
}

Component* CameraMoveComponent::Clone() const
{
    return new CameraMoveComponent(*this);
}

void CameraMoveComponent::Start()
{
	lastMousePosition = Program::GetInstance()->GetCursorPos();
}

void CameraMoveComponent::Update()
{
	Vector2 cursorPos = Program::GetInstance()->GetCursorPos();
	Vector2 mouseDelta = cursorPos - lastMousePosition;
	lastMousePosition = cursorPos;

	//window has to be focused and right mouse button pressed down for this to move
	if (!Program::GetInstance()->GetWindowFocused() || glfwGetMouseButton(Program::GetInstance()->GetWindow(), 1) != GLFW_PRESS)
	{
		Program::GetInstance()->SetCursorLocked(false);
		return;
	}
	else
		Program::GetInstance()->SetCursorLocked(true);

	Vector3 cameraDir = Vector3();
	Transform& t = GetTransform();

	GLFWwindow* window = Program::GetInstance()->GetWindow();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraDir -= t.GetLocalForward();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraDir += t.GetLocalForward();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraDir -= t.GetLocalRight();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraDir += t.GetLocalRight();
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cameraDir -= t.GetLocalUp();
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		cameraDir += t.GetLocalUp();
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		cameraDir *= speedMod;

	//move cam
	t.LocalMove(cameraDir * Program::GetInstance()->GetDeltaTime() * moveSpeed);
	rotation += mouseDelta * -0.0007f * turnSpeed;
	rotation.y = glm::clamp(rotation.y, glm::radians(-90.0f), glm::radians(90.0f));
	//rotate cam
	t.SetLocalRotation(glm::angleAxis(rotation.x, Vector3(0, 1, 0)) * glm::angleAxis(rotation.y, Vector3(1, 0, 0)));
}

void CameraMoveComponent::OnDisable()
{
}

void CameraMoveComponent::OnEnable()
{
}

void CameraMoveComponent::Unload()
{
}
