#pragma once
#include "Graphics.h"
#include "Math.h"

class Transform
{
public:
	Transform();

	void SetPosition(Vector3 position);
	void SetRotation(Quaternion rotation);
	void SetScale(Vector3 scale);
	void UpdateMatrix();

	inline const Vector3& GetPosition() const { return position; }
	inline const Quaternion& GetRotation() const { return rotation; }
	inline const Vector3& GetScale() const { return scale; }
	inline const Matrix4x4& GetMatrix() const { return matrix; }
	
private:
	Matrix4x4 matrix;
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
};

