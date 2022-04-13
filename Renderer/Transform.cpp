#include "Transform.h"
#include "GameObject.h"
#include "Program.h"

Transform::Transform()
{
	scale = Vector3(1, 1, 1); 
	rotation = glm::quat_identity<float, glm::qualifier::defaultp>(); 
	position = Vector3(0, 0, 0); 
	UpdateLocalMatrix();

	parent = nullptr;
	Program::GetInstance()->AddChild(*this);
}

Transform::Transform(Transform& parent)
{
	scale = Vector3(1, 1, 1);
	rotation = glm::quat_identity<float, glm::qualifier::defaultp>();
	position = Vector3(0, 0, 0);
	UpdateLocalMatrix();

	parent.AddChild(*this);
}

#pragma region Local
void Transform::SetLocalPosition(const Vector3& position)
{
	this->position = position;
	UpdateLocalMatrix();
}

void Transform::SetLocalRotation(const Quaternion& rotation)
{
	this->rotation = rotation;
	UpdateLocalMatrix();
}

void Transform::SetLocalScale(const Vector3& scale)
{
	this->scale = scale;
	UpdateLocalMatrix();
}

void Transform::SetLocalScale(float scale)
{
	this->scale = Vector3(scale, scale, scale);
	UpdateLocalMatrix();
}

void Transform::UpdateLocalMatrix()
{
	//ordered as translate, rotate, scale
	//which don't really make sense, surely it should be rotate, then scale, THEN translate, but whatever idk

	localMatrix = glm::translate(glm::identity<Matrix4x4>(), position);
	localMatrix = localMatrix * glm::toMat4(rotation);
	localMatrix = glm::scale(localMatrix, scale);

	UpdateGlobalMatrix();
}

const Vector3& Transform::GetLocalForward() const
{
	return localMatrix[2];
}

const Vector3& Transform::GetLocalUp() const
{
	return localMatrix[1];
}

const Vector3& Transform::GetLocalRight() const
{
	return localMatrix[0];
}
#pragma endregion

#pragma region Global
void Transform::SetPosition(const Vector3& position)
{
	this->position = InverseTransformPoint(position);
	UpdateLocalMatrix();
}

void Transform::SetRotation(const Quaternion& rotation)
{
	this->rotation = InverseTransformRotation(rotation);
	UpdateLocalMatrix();
}

void Transform::UpdateGlobalMatrix()
{
	if (parent)
		globalMatrix = localMatrix * parent->GetGlobalMatrix();
	else
		globalMatrix = localMatrix;

	for (auto child : children)
	{
		child->UpdateGlobalMatrix();
	}
}

void Transform::Rotate(const Quaternion& rot)
{
	//get rotation of inverse matrix
	//this might not actually be the same, we will see

	Quaternion localRot = InverseTransformRotation(rot);
	rotation *= localRot;

	UpdateLocalMatrix();
}

void Transform::Rotate(float rotation, const Vector3& axis)
{
	Vector3 localAxis = Vector4(axis, 0) * GetGlobalToLocalMatrix();

	UpdateLocalMatrix();
}

void Transform::Move(const Vector3& move)
{
	Vector3 localMove = Vector4(move, 1) * GetGlobalToLocalMatrix();
	LocalMove(position);

	UpdateLocalMatrix();
}

const Vector3& Transform::GetForward() const
{
	return globalMatrix[2];
}

const Vector3& Transform::GetUp() const
{
	return globalMatrix[1];
}

const Vector3& Transform::GetRight() const
{
	return globalMatrix[0];
}

const Vector3& Transform::GetPosition() const
{
	//this is a sin
	return *(Vector3*)(&globalMatrix[3]);
}

Quaternion Transform::GetRotation() const
{
	//madre de dios, este es muy caro

	Vector3 scale = GetScale();
	Matrix3x3 rotation(
		Vector3(globalMatrix[0]) / scale.x,
		Vector3(globalMatrix[1]) / scale.y,
		Vector3(globalMatrix[2]) / scale.z
	);

	return glm::quat_cast(rotation);
}

Vector3 Transform::GetScale() const
{
	return Vector3(glm::length(globalMatrix[0]),
		glm::length(globalMatrix[1]),
		glm::length(globalMatrix[2]));
}
#pragma endregion

#pragma region Transform

Matrix4x4 Transform::GetGlobalToLocalMatrix() const
{
	return glm::inverse(globalMatrix);
}
Vector3 Transform::TransformPoint(const Vector3& point) const
{
	return Vector4(point, 1.0) * globalMatrix;
}
Vector3 Transform::InverseTransformPoint(const Vector3& point) const
{
	return Vector4(point, 1.0) * GetGlobalToLocalMatrix();
}
Quaternion Transform::TransformRotation(const Quaternion& rotation) const
{
	return rotation * GetRotation();
}
Quaternion Transform::InverseTransformRotation(const Quaternion& rotation) const
{
	Quaternion inverseRotation = glm::inverse(GetRotation());
	return rotation * inverseRotation;
}
Vector3 Transform::TransformDirection(const Vector3& direction) const
{
	return Vector4(direction, 0.0) * globalMatrix;
}
Vector3 Transform::InverseTransformDirection(const Vector3& direction) const
{
	return Vector4(direction, 0.0) * GetGlobalToLocalMatrix();
}
#pragma endregion

#pragma region Hierarchy
Transform* Transform::GetChild(int index) const
{
	if (index < 0 || index >= children.size())
		return nullptr;
	
	return children[index];
}

void Transform::AddChild(Transform& child)
{
	if (child.parent)
	{
		auto position = std::find(children.begin(), children.end(), &child);
		if (position != children.end())
			children.erase(position);
	}
	else
		Program::GetInstance()->RemoveChild(child);

	child.parent = this;
	children.push_back(&child);
}

void Transform::RemoveChild(int index)
{
	if (index < 0 || index >= children.size())
		return;
	
	children.erase(children.begin() + index);
	
	children[index]->parent = nullptr;
	Program::GetInstance()->AddChild(*children[index]);
}

void Transform::RemoveChild(Transform& child)
{
	auto position = std::find(children.begin(), children.end(), &child);
	if (position != children.end())
	{
		children.erase(position);

		child.parent = nullptr;
		Program::GetInstance()->AddChild(child);
	}
}

bool Transform::IsParentOf(Transform& child)
{
	return child.parent == this;
}

void Transform::SetAttachedGameObject(GameObject* gO)
{
	gameObject = gO;
}
#pragma endregion
