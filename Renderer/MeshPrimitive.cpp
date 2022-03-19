#include "MeshPrimitive.h"
constexpr float RADIUS = 0.25f;

void MeshPrimitive::SetCube(MeshData& data)
{
	//values from https://pastebin.com/DXKEmvap

	Vector3 vertices[8] =
	{
		Vector3(-RADIUS, -RADIUS, -RADIUS),
		Vector3( RADIUS, -RADIUS, -RADIUS),
		Vector3( RADIUS,  RADIUS, -RADIUS),
		Vector3(-RADIUS,  RADIUS, -RADIUS),
		Vector3(-RADIUS, -RADIUS,  RADIUS),
		Vector3( RADIUS, -RADIUS,  RADIUS),
		Vector3( RADIUS,  RADIUS,  RADIUS),
		Vector3(-RADIUS,  RADIUS,  RADIUS)
	};
	GLuint indices[6 * 6] =
	{
		0, 1, 3, 3, 1, 2,
		1, 5, 2, 2, 5, 6,
		5, 4, 6, 6, 4, 7,
		4, 0, 7, 7, 0, 3,
		3, 2, 7, 7, 2, 6,
		4, 5, 0, 0, 5, 1
	};
	data.AllocateMeshData(8, 36);
	data.SetPositions(vertices, 8);
	data.SetIndices(indices, 36);
}

void MeshPrimitive::SetCircle(MeshData& data)
{
	SetCube(data);
}

void MeshPrimitive::SetCylinder(MeshData& data)
{
	SetCube(data);
}
