#include "GameObject.h"

#pragma once
class Component
{
	//https://github.com/cophfe/Pathfinding/blob/main/PathfindingGame/Component.h
	//just base it on this^ (made last year)

public:
	virtual Component* Clone() const = 0;

	void SetEnabled(bool enabled);
	inline bool GetEnabled() const { return enabled; }
	inline GameObject* GetGameObject() const { return gameObject; }

protected:
	friend GameObject;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void OnDisable() = 0;
	virtual void OnEnable() = 0;
	virtual void Unload() = 0;
	void ConnectGameObject(GameObject* connected);

private:
	bool enabled;
	bool initiated;
	GameObject* gameObject;
};

