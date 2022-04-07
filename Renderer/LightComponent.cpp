#include "LightComponent.h"

LightComponent::LightComponent(Vector3 luminance, LightType type)
{
	lightData.luminance = luminance;
	this->type = type;

	switch (type)
	{
	case LightType::DIRECTION:
		lightData.type = 0;
		break;
	case LightType::POINT:
		lightData.type = 1;
		break;
	case LightType::SPOTLIGHT:
		lightData.type = 2;
		break;
	}
}

void LightComponent::SetSpotLightData(float radius, LightAttenuationType type, float minAngleRad, float falloffRadians)
{
	lightData.minAngle = glm::cos(minAngleRad);
	lightData.maxAngle = glm::cos(minAngleRad + falloffRadians);

	SetPointLightData(radius, type);
}

void LightComponent::SetPointLightData(float radius, LightAttenuationType type)
{
	switch (type)
	{
	case LightAttenuationType::INVERSE_SQUARED:
		//describes how the constant linear quadratic formula is physically accurate
		//https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/

		lightData.linear = 2.0f / radius;
		lightData.quadratic = 1.0f / (radius * radius);
		break;
	case LightAttenuationType::LINEAR:
		lightData.linear = 2.0f / radius;
		lightData.quadratic = 1.0f;
		break;
	}
}

float LightComponent::EstimateLightIntensityAtPoint(Vector3 point, const LightDataStruct& lightData)
{
	//this is a very rough estimate since it doesn't have any normal data. could be used to choose what lights are sent to the GPU per object

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
		float attenuation = 1.0 / (1 + lightData.linear * distance + lightData.quadratic * distance * distance);
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
}

void LightComponent::OnEnable()
{
}

void LightComponent::Unload()
{
}

Component* LightComponent::Clone() const
{
	LightComponent* component = new LightComponent(lightData.luminance, type);
	component->lightData = lightData;

	component->SetEnabled(GetEnabled());
    return component;
}
