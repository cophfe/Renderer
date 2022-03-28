#pragma once
#include "Graphics.h"
#include "Math.h"

class Transform
{
public:
	Transform();

	void SetPosition(const Vector3& position);
	void SetRotation(const Quaternion& rotation);
	void SetScale(Vector3 scale);
	void SetScale(float scale);
	void UpdateMatrix();

	Vector3&& GetForward() const;
	Vector3&& GetUp() const;
	Vector3&& GetRight() const;

	inline const Vector3& GetPosition() const { return position; }
	inline const Quaternion& GetRotation() const { return rotation; }
	inline const Vector3& GetScale() const { return scale; }
	inline const Matrix4x4& GetMatrix() const { return matrix; }
	
	inline void Rotate(const Quaternion& rot) { rotation = rot * rotation; UpdateMatrix(); }
	inline void Rotate(float rotation, const Vector3& axis) { this->rotation = glm::angleAxis(rotation, axis) * this->rotation; UpdateMatrix(); }
	inline void Move(const Vector3& move) { position += move; UpdateMatrix(); }
	
private:
	Matrix4x4 matrix;
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
};

