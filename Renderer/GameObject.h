#pragma once
#include <vector>
#include "Component.h"
#include "Transform.h"
class GameObject
{
public:
	void Init();
	void Update();
	void End();

	template<typename T>
	T* AddComponent();
	template<typename T>
	T* GetComponentOfType();
	template<typename T>
	T* DeleteComponentOfType();

	unsigned int GetID();
	bool GetEnabled();

	void SetEnabled(bool enabled);

private:
	static unsigned int idCounter;
	unsigned int id;
	
	bool enabled;
	std::vector<Component*> components;
	Transform transform;

};

