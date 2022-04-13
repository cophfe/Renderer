#include "LightComponent.h"
#include "Program.h"

void LightComponent::Init(Vector3 luminance, LightType type)
{
	this->luminance = luminance;
	lightData.luminance = luminance;
	this->type = type;
	lightData.type = (int)type;
	Program::GetInstance()->GetRenderer().UpdateLights();
}

void LightComponent::Init(Vector3 luminance, float radius, float minAngleRad, float falloffRadians, LightType type)
{
	this->luminance = luminance;
	lightData.radius = radius;
	lightData.luminance = luminance;
	this->type = type;
	lightData.type = (int)type;
	
	lightData.minAngle = glm::cos(minAngleRad);
	lightData.maxAngle = glm::cos(minAngleRad + falloffRadians);
	Program::GetInstance()->GetRenderer().UpdateLights();
}

void LightComponent::Init(Vector3 luminance, float radius, LightType type)
{
	this->luminance = luminance;
	lightData.radius = radius;
	lightData.luminance = luminance;
	this->type = type;
	lightData.type = (int)type;
	Program::GetInstance()->GetRenderer().UpdateLights();
}

void LightComponent::SetSpotLightData(float radius, float minAngleRad, float falloffRadians)
{
	lightData.minAngle = glm::cos(minAngleRad);
	lightData.maxAngle = glm::cos(minAngleRad + falloffRadians);
	SetPointLightData(radius);
}

void LightComponent::SetPointLightData(float radius)
{
	lightData.luminance = luminance * radius * radius;
	Program::GetInstance()->GetRenderer().UpdateLights();
}

float LightComponent::EstimateLightIntensityAtPoint(Vector3 point, const LightDataStruct& lightData)
{
	//this is a very rough estimate since it doesn't have any normal data. could be used to choose what lights are sent to the GPU per object, if there are more than 8 lights

	float intensity = 0.21f * lightData.luminance.r + 0.72 * lightData.luminance.g + 0.07 * lightData.luminance.b;
	float distance = glm::length(lightData.position - point);

	switch ((LightType)lightData.type)
	{
	case LightType::SPOTLIGHT:
	{
		Vector3 rayDirection = (lightData.position - point) / distance;
		float theta = glm::dot(rayDirection, lightData.direction);
		float epsilon = lightData.minAngle - lightData.maxAngle;
		float falloff = glm::clamp((theta - lightData.maxAngle) / epsilon, 0.0f, 1.0f);
		intensity *= falloff;
	}
	case LightType::POINT:
	{
		float attenuation = 1.0f / (distance * distance);
		intensity *= attenuation;
		break;
	}
	}

	return intensity;
}

const LightDataStruct& LightComponent::GetLightData()
{
	lightData.position = GetGameObject()->GetTransform().GetPosition();
	lightData.direction = GetGameObject()->GetTransform().GetForward();
	
	return lightData;
}

void LightComponent::Start()
{

}

void LightComponent::Update()
{
}

void LightComponent::OnDisable()
{
	Program::GetInstance()->GetRenderer().DeregisterLight(this);
}

void LightComponent::OnEnable()
{
	Program::GetInstance()->GetRenderer().RegisterLight(this);
}

void LightComponent::Unload()
{

}

Component* LightComponent::Clone() const
{
	LightComponent* component = new LightComponent(*this);
    return component;
}
