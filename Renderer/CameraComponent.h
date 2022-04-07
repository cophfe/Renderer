#pragma once
#include "Component.h"

class CameraComponent : public Component
{
private:
	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;
	virtual Component* Clone() const override;
};

