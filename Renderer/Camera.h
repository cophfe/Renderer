#pragma once
#include "Math.h"
#include "Graphics.h"
#include "Transform.h"

class Camera
{
public:
	Camera(float fov, float aspect, float near, float far) : fov(fov), aspect(aspect), near(near), far(far) { UpdateCamera(); }

	const Matrix4x4& GetProjectionMatrix() const;
	const Matrix4x4& GetViewMatrix() const;
	const Matrix4x4& GetViewProjectionMatrix() const;
	void UpdateCamera();
	inline Transform& GetTransform() { return transform; }

	void SetAspect(Vector2 size);
	inline void SetAspect(float aspect) { this->aspect = aspect; }
	inline void SetNear(float near) { this->near = near; }
	inline void SetFar(float far) { this->far = far; }

private:
	float fov;
	float aspect;
	float near = 0.01f;
	float far = 100.0f;

	Matrix4x4 viewProjectionMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;

	Transform transform;
};

