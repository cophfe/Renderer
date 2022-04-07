#include "MeshPrimitive.h"
constexpr float RADIUS = 0.25f;

void MeshPrimitive::SetCube(MeshData& data, float radius)
{
	//values from https://pastebin.com/DXKEmvap

	MeshData::VertexData vertices[24] = {
		//BACK
		MeshData::VertexData(Vector3(-radius,  radius, -radius),  Vector2(0.0f, 1.0f)), //3
		MeshData::VertexData(Vector3( radius,  radius, -radius),  Vector2(1.0f, 1.0f)), //2
		MeshData::VertexData(Vector3( radius, -radius, -radius),  Vector2(1.0f, 0.0f)), //1
		MeshData::VertexData(Vector3(-radius, -radius, -radius),  Vector2(0.0f, 0.0f)), //0
		//FRONT
		MeshData::VertexData(Vector3(-radius, -radius,  radius),  Vector2(0.0f, 0.0f)), //4
		MeshData::VertexData(Vector3( radius, -radius,  radius),  Vector2(1.0f, 0.0f)), //5
		MeshData::VertexData(Vector3( radius,  radius,  radius),  Vector2(1.0f, 1.0f)), //6
		MeshData::VertexData(Vector3(-radius,  radius,  radius),  Vector2(0.0f, 1.0f)), //7
		//RIGHT
		MeshData::VertexData(Vector3(-radius,  radius,  radius),  Vector2(1.0f, 0.0f)), //8
		MeshData::VertexData(Vector3(-radius,  radius, -radius),  Vector2(1.0f, 1.0f)), //9
		MeshData::VertexData(Vector3(-radius, -radius, -radius),  Vector2(0.0f, 1.0f)), //10
		MeshData::VertexData(Vector3(-radius, -radius,  radius),  Vector2(0.0f, 0.0f)), //11
		//LEFT
		MeshData::VertexData(Vector3( radius, -radius,  radius),  Vector2(0.0f, 0.0f)), //15
		MeshData::VertexData(Vector3( radius, -radius, -radius),  Vector2(0.0f, 1.0f)), //14
		MeshData::VertexData(Vector3( radius,  radius, -radius),  Vector2(1.0f, 1.0f)), //13
		MeshData::VertexData(Vector3( radius,  radius,  radius),  Vector2(1.0f, 0.0f)), //12
		//DOWN
		MeshData::VertexData(Vector3(-radius, -radius, -radius),  Vector2(0.0f, 1.0f)), //16
		MeshData::VertexData(Vector3( radius, -radius, -radius),  Vector2(1.0f, 1.0f)), //17
		MeshData::VertexData(Vector3( radius, -radius,  radius),  Vector2(1.0f, 0.0f)), //18
		MeshData::VertexData(Vector3(-radius, -radius,  radius),  Vector2(0.0f, 0.0f)), //19
		//UP
		MeshData::VertexData(Vector3(-radius,  radius,  radius),  Vector2(0.0f, 0.0f)), //23
		MeshData::VertexData(Vector3( radius,  radius,  radius),  Vector2(1.0f, 0.0f)), //22
		MeshData::VertexData(Vector3( radius,  radius, -radius),  Vector2(1.0f, 1.0f)), //21
		MeshData::VertexData(Vector3(-radius,  radius, -radius),  Vector2(0.0f, 1.0f)), //20
	};

	MeshData::Index indices[36] =
	{
		0,  1,  2,
		2,  3,  0,
		4,  5,  6,
		6,  7,  4,
		8,  9,  10,
		10, 11, 8,
		12, 13, 14,
		14, 15, 12,
		16, 17, 18,
		18, 19, 16,
		20, 21, 22,
		22, 23, 20
	};
	data.AllocateMeshData(24, 36);
	data.SetVerticesData(vertices, 24);
	data.SetIndices(indices, 36);
	
	data.CalculateNormalTangentBitangents(false);
}

void MeshPrimitive::SetCircle(MeshData& data, float radius)
{
	SetCube(data);
}

void MeshPrimitive::SetCylinder(MeshData& data, float radius, float height)
{
	SetCube(data);
}
