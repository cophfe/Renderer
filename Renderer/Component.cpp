#include "Component.h"

//softset is true when the gameobject is disabled/enabled but the component itself isn't
void Component::SetEnabled(bool enabled, bool softset)
{
	if (enabled == this->enabled)
		return;

	if (!softset)
		this->enabled = enabled;
	
	if (enabled)
	{
		OnEnable();
	
		if (!initiated)
		{
			initiated = true;
			Start();
		}
	}
	else
		OnDisable();
	
}

void Component::ConnectGameObject(GameObject* connected)
{
	gameObject = connected;
}
