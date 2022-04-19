#pragma once
#include "Component.h"

class CameraComponent : public Component
{
public:
	void Init(float fov, float aspect, float near = 0.01f, float far = 100, int priority = 0) 
	{
		this->fov = fov;
		this->aspect = aspect;
		this->near = near;
		this->far = far;
		this->priority = priority;
		UpdateCamera(); 
		UpdateProjection();
	}
	inline int GetPriority() const { return priority; }
	inline float GetAspect() const { return aspect; }
	inline float GetFOV() const { return fov; }
	const Matrix4x4& GetProjectionMatrix() const;
	const Matrix4x4& GetViewMatrix() const;
	const Matrix4x4& GetViewProjectionMatrix() const;
	void UpdateCamera();

	void SetAspect(Vector2 size);
	inline void SetAspect(float aspect) { this->aspect = aspect; UpdateProjection(); }
	inline void SetNear(float near) { this->near = near; UpdateProjection(); }
	inline void SetFar(float far) { this->far = far; UpdateProjection(); }
	inline void SetFOV(float fov) { this->fov = fov; UpdateProjection(); }
	inline void SetPriority(int priority) { this->priority = priority; }

private:
	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;
	virtual Component* Clone() const override;

	void UpdateProjection();

	float fov;
	float aspect;
	float near;
	float far;
	int priority;

	Matrix4x4 viewProjectionMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
};

