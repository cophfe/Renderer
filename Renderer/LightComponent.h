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
	float minAngle;
	Vector3 luminance;
	float maxAngle;
	Vector3 direction;
	int type = 0;
};

class LightComponent : public Component
{
public:
	void Init(Vector3 luminance, LightType type);
	void Init(Vector3 luminance, float radius, float minAngleRad, float falloffRadians, 
		LightType type = LightType::SPOTLIGHT);
	void Init(Vector3 luminance, float radius, 
		LightType type = LightType::POINT);

	void SetSpotLightData(float radius, float minAngleRad, float falloffRadians);
	void SetPointLightData(float radius);
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

	Vector3 luminance;
	LightDataStruct lightData;
	LightType type;
};

