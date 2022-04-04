#include "Component.h"

void Component::SetEnabled(bool enabled)
{
	this->enabled = enabled;
	if (enabled)
	{
		OnEnable();
	
		if (!initiated)
		{
			initiated = true;
			Init();
		}
	}
	else
		OnDisable();
	
}

void Component::ConnectGameObject(GameObject* connected)
{
	gameObject = connected;
}
