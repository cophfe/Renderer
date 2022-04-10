#pragma once
#include "Component.h"

class CameraMoveComponent : public Component
{
public:
	void Init(float moveSpeed = 2.0f, float turnSpeed = 1.0f, float speedMod = 2.0f);
private:
	// Inherited via Component
	virtual Component* Clone() const override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;

	float moveSpeed;
	float turnSpeed;
	float speedMod;

	Vector2 lastMousePosition;
	Vector2 rotation;
};

