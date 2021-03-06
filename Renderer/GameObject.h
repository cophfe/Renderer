#pragma once
#include <vector>
#include "Transform.h"
class Component;

class GameObject
{
public:
	static GameObject* Create(Transform* parent = nullptr);

	void Start();
	void Update();
	void Unload();
	void UnloadHierarchy();
	void SetEnabled(bool enabled);

	template<typename T>
	T* AddComponent();
	template<typename T>
	T* GetComponentOfType();
	template<typename T>
	std::vector<T*> GetComponentsOfType();
	template<typename T>
	void DeleteComponentOfType();
	template<typename T>
	void DeleteComponentsOfType();

	inline unsigned int GetID() { return id; }
	inline bool GetEnabled() { return enabled; }
	Transform& GetTransform() { return transform; }
	const Transform& ReadTransform() const { return transform; }

	~GameObject();
	GameObject(const GameObject& other) = delete;
	GameObject& operator=(const GameObject& other) = delete;
	GameObject(GameObject&& other);
	GameObject& operator=(GameObject&& other);
	GameObject* Copy();
private:
	static unsigned int idCounter;
	int id;

	GameObject() = default;

	bool enabled;
	std::vector<Component*> components;
	Transform transform;

};

template<typename T>
inline T* GameObject::AddComponent() //this will be broken if the type given does not inherit from component
{
	T* component = new T();
	((Component*)component)->ConnectGameObject(this);
	components.push_back(component);

	//component->SetEnabled(true);

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
inline void GameObject::DeleteComponentOfType()
{
	size_t i = 0;

	for (auto& component : components)
	{
		if (typeid(*component) == typeid(T))
		{
			component->SetEnabled(false);
			component->Unload();
			delete component;
			components.erase(components.begin() + i);
			break;
		}

		i++;
	}
}

template<typename T>
inline void GameObject::DeleteComponentsOfType()
{
	size_t i = 0;
	for (auto& component : components)
	{
		if (typeid(*component) == typeid(T))
		{
			component->SetEnabled(false);
			component->Unload();
			delete component;
			components.erase(components.begin() + i);

			DeleteComponentsOfType<T>();
			break;
		}

		i++;
	}
}


