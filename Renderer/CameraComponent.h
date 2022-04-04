#pragma once
#include "Component.h"

class CameraComponent : public Component
{
private:
	// Inherited via Component
	virtual void Init() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;
	virtual Component* Clone() override;
};

