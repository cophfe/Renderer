#pragma once
#include "Graphics.h"
#include "MeshData.h"

struct SubMesh
{
public:
	GLuint vertexArray;
	//vertex info is stored in one big array, but is not interleaved.
	GLuint vertexBuffer;
	GLuint elementBuffer;

	MeshData* data; //not usually stored, can be stored
	uint32_t verticesCount;
	uint32_t indicesCount;

	void Render() const;
	inline MeshData* GetStoredMeshData() { return data; }
};

