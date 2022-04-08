#include "LightComponent.h"
#include "Program.h"

void LightComponent::Init(Vector3 luminance, LightType type)
{
	lightData.luminance = luminance;
	this->type = type;
	lightData.type = (int)type;
}

void LightComponent::Init(Vector3 luminance, float radius, float minAngleRad, float falloffRadians, LightType type, LightAttenuationType attenuationType)
{
	lightData.luminance = luminance;
	this->type = type;
	lightData.type = (int)type;

	switch (attenuationType)
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

	lightData.minAngle = glm::cos(minAngleRad);
	lightData.maxAngle = glm::cos(minAngleRad + falloffRadians);
}

void LightComponent::Init(Vector3 luminance, float radius, LightType type, LightAttenuationType attenuationType)
{
	lightData.luminance = luminance;
	this->type = type;
	lightData.type = (int)type;
	switch (attenuationType)
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

void LightComponent::SetSpotLightData(float radius, float minAngleRad, float falloffRadians, LightAttenuationType type)
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
		float attenuation = 1.0f / (1 + lightData.linear * distance + lightData.quadratic * distance * distance);
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
