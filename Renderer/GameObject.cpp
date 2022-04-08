#include "GameObject.h"
#include "Component.h"
#include "Program.h"

unsigned int GameObject::idCounter = 1;

GameObject* GameObject::Create(Transform* parent)
{
	GameObject* gO = new GameObject();
	gO->id = idCounter;
	gO->enabled = true;
	
	if (parent)
		parent->AddChild(gO->GetTransform());
	idCounter++;

	Program::GetInstance()->RegisterGameObject(gO);
	return gO;
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

void GameObject::Unload() //called when deleting all objects at once, so no hirarchy changes needed
{
	for (auto& component : components)
	{
		component->Unload();
		delete component;
	}
	components.clear();
}

void GameObject::UnloadHierarchy() //called when this object specifically is being deleted
{
	for (auto& component : components)
	{
		component->Unload();
		delete component;
	}
	components.clear();

	for (auto* child : transform.GetChildArray())
	{
		child->GetGameObject()->UnloadHierarchy();
		delete child;
	}
	transform.children.clear();

	Program::GetInstance()->DeregisterGameObject(this);
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

GameObject::GameObject(GameObject&& other)
{
	Program::GetInstance()->DeregisterGameObject(&other);
	Program::GetInstance()->RegisterGameObject(this);

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
	UnloadHierarchy();

	Program::GetInstance()->DeregisterGameObject(&other);
	Program::GetInstance()->RegisterGameObject(this);

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
	gO->transform = transform;
	gO->components = components;
	for (size_t i = 0; i < components.size(); i++)
	{
		gO->components[i] = components[i]->Clone();
	}

	gO->transform = transform;

	for (size_t i = 0; i < transform.children.size(); i++)
	{
		gO->transform.children[i] = &transform.children[i]->GetGameObject()->Copy()->GetTransform();
	}
	gO->enabled = enabled;
	gO->id = idCounter;
	idCounter++;
	Program::GetInstance()->RegisterGameObject(this);

	return nullptr;
}
