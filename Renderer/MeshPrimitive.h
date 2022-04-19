#pragma once
#include "MeshData.h"

class MeshPrimitive
{
public:
	static void SetCube(MeshData& data, float radius = 0.5f, bool invert = false, GLushort flags = 0xFFFF);
	static void SetCircle(MeshData& data, float radius = 0.5f, bool invert = false, GLushort flags = 0xFFFF);
};