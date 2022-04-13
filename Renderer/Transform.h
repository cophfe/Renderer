#pragma once
#include "Graphics.h"
#include "Math.h"
#include <vector>
class GameObject;

class Transform
{
public:
	Transform();
	Transform(Transform& parent);
	//Local
	void SetLocalPosition(const Vector3& position);
	void SetLocalRotation(const Quaternion& rotation);
	void SetLocalScale(const Vector3& scale);
	void SetLocalScale(float scale);
	void UpdateLocalMatrix();

	inline void LocalRotate(const Quaternion& rot) { rotation = rot * rotation; UpdateLocalMatrix(); }
	inline void LocalRotate(float rotation, const Vector3& axis) { this->rotation = glm::angleAxis(rotation, axis) * this->rotation; UpdateLocalMatrix(); }
	inline void LocalMove(const Vector3& move) { position += move; UpdateLocalMatrix(); }

	const Vector3& GetLocalForward() const;
	const Vector3& GetLocalUp() const;
	const Vector3& GetLocalRight() const;
	inline const Vector3& GetLocalPosition() const { return position; }
	inline const Quaternion& GetLocalRotation() const { return rotation; }
	inline const Vector3& GetLocalScale() const { return scale; }
	inline const Matrix4x4& GetLocalMatrix() const { return localMatrix; }
	//
	
	//Global
	void SetPosition(const Vector3& position);
	void SetRotation(const Quaternion& rotation);
	void UpdateGlobalMatrix();
	
	void Rotate(const Quaternion& rot);
	void Rotate(float rotation, const Vector3& axis);
	void Move(const Vector3& move);

	const Vector3& GetForward() const;
	const Vector3& GetUp() const;
	const Vector3& GetRight() const;
	const Vector3& GetPosition() const;
	Quaternion GetRotation() const;
	Vector3 GetScale() const;
	inline const Matrix4x4& GetGlobalMatrix() const { return globalMatrix; }
	Matrix4x4 GetGlobalToLocalMatrix() const;
	//

	//transform
	Vector3 TransformPoint(const Vector3& point) const;
	Vector3 InverseTransformPoint(const Vector3& point) const;
	Quaternion TransformRotation(const Quaternion& rotation) const;
	Quaternion InverseTransformRotation(const Quaternion& rotation) const;
	Vector3 TransformDirection(const Vector3& direction) const;
	Vector3 InverseTransformDirection(const Vector3& direction) const;
	//

	//Hierarchy
	Transform* GetChild(int index) const;
	Transform* GetParent() const { return parent; }
	inline const std::vector<Transform*>& GetChildArray() { return children; }

	void AddChild(Transform& child);
	void RemoveChild(int index);
	void RemoveChild(Transform& child);

	bool IsParentOf(Transform& child);
	//

	inline GameObject* GetGameObject() const { return gameObject; }	
private:
	friend GameObject;
	void SetAttachedGameObject(GameObject* gO);

	GameObject* gameObject;
	//hierarchy
	Transform* parent;
	std::vector<Transform*> children;

	//transform matrices
	Matrix4x4 globalMatrix;
	Matrix4x4 localMatrix;

	//local values
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
};

