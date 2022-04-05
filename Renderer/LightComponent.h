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
	int : 32;
	float linear;
	float quadratic;
	float minAngle;
	float maxAngle;
	int type = 0;
	int : 32;
	int : 32;
	int : 32;
};

enum class LightAttenuationType
{
	INVERSE_SQUARED,
	LINEAR

};

class LightComponent : public Component
{
public:
	LightComponent(Vector3 luminance, LightType type);
	void SetSpotLightData(float radius, LightAttenuationType type, float minAngleRad, float falloffRadians);
	void SetPointLightData(float radius, LightAttenuationType type);
	static float EstimateLightIntensityAtPoint(Vector3 point, const LightDataStruct& lightData);

	const LightDataStruct& GetLightData();

private:
	// Inherited via Component
	virtual void Init() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;
	virtual Component* Clone() const override;

	LightDataStruct lightData;
	LightType type;
};

