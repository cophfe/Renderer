#include "GameObject.h"
#include "Component.h"

unsigned int GameObject::idCounter = 1;

void GameObject::Init()
{
	for (auto& component : components)
	{
		//don't change this, it is correct (even if it doesn't look it)
		if (component->enabled)
		{
			component->SetEnabled(true);
		}
	}
}

void GameObject::Update()
{
	for (auto& component : components)
	{
		component->Update();
	}
}

void GameObject::Unload()
{
	for (auto& component : components)
	{
		component->Unload();
	}
}

void GameObject::SetEnabled(bool enabled)
{
	if (enabled)
	{
		for (auto& component : components)
		{
			component->SetEnabled(true);
		}
	}
	else
	{
		for (auto& component : components)
		{
			component->SetEnabled(false);
		}
	}
}

GameObject::GameObject()
{
	id = idCounter;
	idCounter++;
}

GameObject::~GameObject()
{
	if (id == 0)
		return;

	for (auto& component : components)
	{
		delete component;
	}
	components.clear();
	id = 0;
}

GameObject::GameObject(const GameObject& other)
{
	components = other.components;

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i] = other.components[i]->Clone();
	}
	
	transform = other.transform;
	enabled = other.enabled;

	id = idCounter;
	idCounter++;

}

GameObject& GameObject::operator=(const GameObject& other)
{
	for (auto& component : components)
	{
		delete component;
	}

	components = other.components;

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i] = other.components[i]->Clone();
	}

	transform = other.transform;
	enabled = other.enabled;
	
	return *this;
}

GameObject::GameObject(GameObject&& other)
{
	id = other.id;
	components = other.components;
	transform = other.transform;
	enabled = other.enabled;
	other.components.clear();
	other.id = 0;
}

GameObject& GameObject::operator=(GameObject&& other)
{
	for (auto& component : components)
	{
		delete component;
	}

	id = other.id;
	components = other.components;
	transform = other.transform;
	enabled = other.enabled;
	other.components.clear();
	other.id = 0;
	return *this;
}
