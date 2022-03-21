#include "Transform.h"

Transform::Transform()
{
	scale = Vector3(1, 1, 1); 
	rotation = glm::quat_identity<float, glm::qualifier::defaultp>(); 
	position = Vector3(0, 0, 0); 
	UpdateMatrix();
};

void Transform::SetPosition(const Vector3& position)
{
	this->position = position;
}

void Transform::SetRotation(const Quaternion& rotation)
{
	this->rotation = rotation;
}

void Transform::SetScale(Vector3 scale)
{
	this->scale = scale;
}

void Transform::UpdateMatrix()
{
	//ordered as translate, rotate, scale
	//which don't really make sense, surely it should be rotate, then scale, THEN translate, but whatever idk

	matrix = glm::translate(glm::identity<Matrix4x4>(), position);
	matrix = matrix * glm::toMat4(rotation);
	matrix = glm::scale(matrix, scale);

}

Vector3&& Transform::GetForward() const
{
	return matrix[2];
}

Vector3&& Transform::GetUp() const
{
	return matrix[1];
}

Vector3&& Transform::GetRight() const
{
	return matrix[0];
}
