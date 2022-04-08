#pragma once
#include "Component.h"

class CameraMoveComponent : public Component
{
public:
	void Init(float moveSpeed, float turnSpeed);
private:
	// Inherited via Component
	virtual Component* Clone() const override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;
};

