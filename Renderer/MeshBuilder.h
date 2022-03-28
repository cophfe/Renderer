#pragma once
#include "MeshData.h"
#include "Object.h"
#include "Material.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <stdexcept>

class MeshBuilder
{
public:
	static MeshData* LoadMeshData(int& meshCount, const char* path);
	static MeshData* LoadMeshData(int& meshCount, void* buffer, size_t size);
	static Object* AutoConstructObject(const char* path, Material* litMaterial);

	static void FreeMeshArray(MeshData*& data, int meshCount);
private:
	static MeshData* LoadMesh(int& meshCount, const aiScene* scene);
};

