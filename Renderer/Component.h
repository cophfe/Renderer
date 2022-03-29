#include "GameObject.h"

#pragma once
class Component
{
	//https://github.com/cophfe/Pathfinding/blob/main/PathfindingGame/Component.h
	//just base it on this^ (made last year)

public:
	virtual void Init();
	virtual void Update();
	virtual void OnDisable();
	virtual void OnEnable();
	virtual void Unload();

	void SetEnabled(bool enabled);
	bool GetEnabled();

private:
	friend GameObject;
	void ConnectGameObject(GameObject* connected);

	bool enabled;
	GameObject* gameObject;
};

