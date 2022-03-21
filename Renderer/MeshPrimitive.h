#pragma once
#include "MeshData.h"

class MeshPrimitive
{
public:
	static void SetCube(MeshData& data, float radius = 0.5f);
	static void SetCircle(MeshData& data, float radius = 0.5f);
	static void SetCylinder(MeshData& data, float radius = 0.5f, float height = 1.0f);
};