#pragma once
#include "MeshData.h"
#include "Material.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <stdexcept>

class MeshBuilder
{
public:
	//useful for when you don't want to reuse the meshdata (most of the time)
	static Mesh* LoadMeshFromPath(const char* path);

	//useful for if you want to reuse the mesh data, however will cause memory leaks if you don't free it
	//it is usually better to reuse the mesh instead of reusing the mesh data, unless you are modifying the mesh data
	static MeshData* LoadMeshData(int& meshCount, const char* path);
	static MeshData* LoadMeshData(int& meshCount, void* buffer, size_t size);
	static void FreeMeshArray(MeshData*& data, int meshCount);
private:
	static MeshData* LoadMesh(int& meshCount, const aiScene* scene);
};

