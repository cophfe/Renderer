#include "CameraMoveComponent.h"

void CameraMoveComponent::Init(float moveSpeed, float turnSpeed)
{
}

Component* CameraMoveComponent::Clone() const
{
    return new CameraMoveComponent(*this);
}

void CameraMoveComponent::Start()
{
}

void CameraMoveComponent::Update()
{
}

void CameraMoveComponent::OnDisable()
{
}

void CameraMoveComponent::OnEnable()
{
}

void CameraMoveComponent::Unload()
{
}
