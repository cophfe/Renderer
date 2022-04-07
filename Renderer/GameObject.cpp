#include "GameObject.h"
#include "Component.h"
#include "Transform.h"

unsigned int GameObject::idCounter = 1;

GameObject* GameObject::Create(Transform* parent)
{
	GameObject* gO = new GameObject();
	gO->id = idCounter;
	gO->enabled = true;
	
	if (parent)
		parent->AddChild(&gO->GetTransform());
	idCounter++;
}

void GameObject::Start()
{
	transform.SetAttachedGameObject(this);

	for (auto& component : components)
	{
		//don't change this, it is correct (even if it doesn't look it)
		if (component->enabled)
		{
			component->enabled = false;
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
		delete component;
	}
	components.clear();

	for (auto& child : transform.children)
	{
		child->gameObject->Unload();
		delete child->gameObject;
	}
	transform.children.clear();
}

void GameObject::SetEnabled(bool enabled)
{
	if (enabled)
	{
		for (auto& component : components)
		{
			component->SetEnabled(true, true);
		}
	}
	else
	{
		for (auto& component : components)
		{
			component->SetEnabled(false, true);
		}
	}

	for (auto& child : transform.children)
	{
		child->gameObject->SetEnabled(enabled);
	}
}

GameObject::~GameObject()
{
	//call unload to unload an object
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
	Unload();

	components = other.components;

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i] = other.components[i]->Clone();
	}

	for (auto& child : transform.children)
	{

	}

	transform = other.transform;
	enabled = other.enabled;
	
	return *this;
}

GameObject::GameObject(GameObject&& other)
{
	other.transform.SetAttachedGameObject(this);
	id = other.id;
	components = other.components;
	transform = other.transform;
	enabled = other.enabled;

	other.transform = Transform();
	other.components.clear();
	other.id = 0;
}

GameObject& GameObject::operator=(GameObject&& other)
{
	Unload();

	id = other.id;
	components = other.components;
	transform = other.transform;
	enabled = other.enabled;

	other.transform = Transform();
	other.components.clear();
	other.id = 0;
	
	return *this;
}

GameObject* GameObject::Copy()
{
	GameObject* gO = new GameObject();
	return nullptr;
}
