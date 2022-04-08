#include "CameraComponent.h"
#include "Program.h"

void CameraComponent::Start()
{
}

void CameraComponent::Update()
{
}

void CameraComponent::OnDisable()
{
	Program::GetInstance()->GetRenderer().DeregisterCamera(this);
}

void CameraComponent::OnEnable()
{
	Program::GetInstance()->GetRenderer().RegisterCamera(this);
}

void CameraComponent::Unload()
{
}

Component* CameraComponent::Clone() const
{
	return new CameraComponent(*this);
}

void CameraComponent::UpdateProjection()
{
	projectionMatrix = glm::perspective(fov, aspect, near, far);
	auto& r = Program::GetInstance()->GetRenderer();
}

const Matrix4x4& CameraComponent::GetProjectionMatrix() const
{
	return projectionMatrix;
}

const Matrix4x4& CameraComponent::GetViewMatrix() const
{
	return viewMatrix;
}

const Matrix4x4& CameraComponent::GetViewProjectionMatrix() const
{
	return viewProjectionMatrix;
}

void CameraComponent::UpdateCamera()
{
	viewMatrix = glm::inverse(GetTransform().GetLocalMatrix());
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void CameraComponent::SetAspect(Vector2 size)
{
	aspect = size.x / size.y;
	UpdateProjection();
}