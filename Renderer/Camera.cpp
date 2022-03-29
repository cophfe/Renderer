#include "Camera.h"

const Matrix4x4& Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

const Matrix4x4& Camera::GetViewMatrix() const
{
	return viewMatrix;
}

const Matrix4x4& Camera::GetViewProjectionMatrix() const
{
	return viewProjectionMatrix;
}

void Camera::UpdateCamera()
{
	viewMatrix = glm::inverse(transform.GetLocalMatrix());
	projectionMatrix = glm::perspective(fov, aspect, near, far);
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::SetAspect(Vector2 size)
{
	aspect = size.x / size.y;
}
