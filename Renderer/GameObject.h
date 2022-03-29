#pragma once
#include <vector>
#include "Transform.h"

class Component;

class GameObject
{
public:
	void Init();
	void Update();
	void End();
	void SetEnabled(bool enabled);

	template<typename T>
	T* AddComponent();
	template<typename T>
	T* GetComponentOfType();
	template<typename T>
	T* DeleteComponentOfType();
	template<typename T>
	std::vector<T*> GetComponentsOfType();

	inline unsigned int GetID() { return id; }
	inline bool GetEnabled() { return enabled; }


private:
	static unsigned int idCounter;
	unsigned int id;
	
	bool enabled;
	std::vector<Component*> components;
	Transform transform;

};

template<typename T>
inline T* GameObject::AddComponent() //this will be broken if the type given does not inherit from component
{
	T* component = new T();
	((Component*)component)->SetGameObject(this);
	components.push_back(component);
	return component;
}
template<typename T>
inline T* GameObject::GetComponentOfType()
{
	for (auto& component : components)
	{
		if (typeid(*component) == typeid(T))
		{
			return (T*)component;
		}
	}
	return nullptr;
}

template<typename T>
inline std::vector<T*> GameObject::GetComponentsOfType()
{
	std::vector<T*> types;

	for (auto& component : components)
	{
		if (typeid(*component) == typeid(T))
		{
			types.push_back(component);
		}
	}

	return types;
}

template<typename T>
inline T* GameObject::DeleteComponentOfType()
{
	/*for (auto& component : components)
	{
		if (typeid(*component) == typeid(T))
		{
			component->unload();
			delete component;
			components.remove(component);
			break;
		}
	}*/
	return nullptr;
}


