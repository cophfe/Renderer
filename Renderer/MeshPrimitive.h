#pragma once
#include "MeshData.h"

class MeshPrimitive
{
public:
	static void SetCube(MeshData& data);
	static void SetCircle(MeshData& data);
	static void SetCylinder(MeshData& data);
};