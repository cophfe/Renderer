#pragma once
#include "Component.h"

enum class LightType
{
	DIRECTION = 0,
	POINT = 1,
	SPOTLIGHT = 2
};

struct LightDataStruct //aligned to 16
{
	Vector3 position;
	int : 32;
	Vector3 luminance;
	int : 32;
	Vector3 direction;
	float linear;
	float quadratic;
	float minAngle;
	float maxAngle;
	int type = 0;
};

enum class LightAttenuationType
{
	INVERSE_SQUARED,
	LINEAR

};

class LightComponent : public Component
{
public:
	void Init(Vector3 luminance, LightType type);
	void Init(Vector3 luminance, float radius, float minAngleRad, float falloffRadians, 
		LightType type = LightType::SPOTLIGHT, LightAttenuationType attenuationType = LightAttenuationType::INVERSE_SQUARED);
	void Init(Vector3 luminance, float radius, 
		LightType type = LightType::POINT, LightAttenuationType attenuationType = LightAttenuationType::INVERSE_SQUARED);

	void SetSpotLightData(float radius, float minAngleRad, float falloffRadians, LightAttenuationType attenuationType = LightAttenuationType::INVERSE_SQUARED);
	void SetPointLightData(float radius, LightAttenuationType attenuationType = LightAttenuationType::INVERSE_SQUARED);
	static float EstimateLightIntensityAtPoint(Vector3 point, const LightDataStruct& lightData);

	const LightDataStruct& GetLightData();

private:
	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;
	virtual Component* Clone() const override;

	LightDataStruct lightData;
	LightType type;
};

