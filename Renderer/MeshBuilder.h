#pragma once
#include "MeshData.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <stdexcept>

class MeshBuilder
{
public:
	static MeshData* LoadMeshData(int& meshCount, const char* path);
	static MeshData* LoadMeshData(int& meshCount, void* buffer, size_t size);

	static void FreeMeshArray(MeshData*& data, int meshCount);
private:
	static MeshData* LoadMesh(int& meshCount, const aiScene* scene);
};

