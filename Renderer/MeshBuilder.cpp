#include "MeshBuilder.h"
#include "Program.h"

MeshData* MeshBuilder::LoadMeshData(int& meshCount, const char* path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_CalcTangentSpace |
		aiProcess_GenNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType 
	); //should ONLY have triangles 
	//currently generates tangents that aren't being used
	
	if (scene == nullptr) {
		auto* msg = importer.GetErrorString();
		throw std::runtime_error(msg);
	}

	return LoadMesh(meshCount, scene);

	//importer destructor will clean up stuff hopefully. else, use aiReleaseImport(scene);
}

MeshData* MeshBuilder::LoadMeshData(int& meshCount, void* buffer, size_t size)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(buffer, size,
		aiProcessPreset_TargetRealtime_Fast);

	if (scene == nullptr) {
		auto* msg = importer.GetErrorString();
		throw std::runtime_error(msg);
	}

	return LoadMesh(meshCount, scene);
}

void MeshBuilder::FreeMeshArray(MeshData*& data, int meshCount)
{
	delete[] data;
	data = nullptr;
}

MeshData* MeshBuilder::LoadMesh(int& meshCount, const aiScene* scene)
{
	meshCount = scene->mNumMeshes;
	MeshData* meshDatas = new MeshData[meshCount];

	for (size_t i = 0; i < meshCount; i++)
	{
		MeshData& mD = meshDatas[i];
		aiMesh& mesh = *(scene->mMeshes[i]);

		size_t vCount = mesh.mNumVertices;
		size_t iCount = mesh.mNumFaces * 3;
		mD.AllocateMeshData(vCount, iCount);

		if (mesh.HasPositions())
			mD.SetPositions((Vector3*)mesh.mVertices, vCount);
		if (mesh.HasNormals())
		{
			mD.SetNormals((Vector3*)mesh.mNormals, vCount);
		}
		if (mesh.HasTangentsAndBitangents())
		{
			mD.SetTangents((Vector3*)mesh.mTangents, vCount);
			mD.SetBitangents((Vector3*)mesh.mBitangents, vCount);
		}
		if (mesh.HasTextureCoords(0))
		{
			//we only care about the first texCoord right now because that's all MeshData supports
			for (size_t j = 0; j  < vCount; j ++)
			{
				//this ignores the last component of the 3D texCoord, cuz we only use 2D texCoords
				mD.texCoords[j] = Vector2(mesh.mTextureCoords[0][j].x, mesh.mTextureCoords[0][j].y);
			}
			
		}

		unsigned int offset = 0;
		for (size_t j = 0; j < mesh.mNumFaces; j++)
		{
			memcpy(mD.indices + offset, mesh.mFaces[j].mIndices, mesh.mFaces[j].mNumIndices * sizeof(unsigned int));
			offset += mesh.mFaces[j].mNumIndices;
		}
	}

	return meshDatas;
}
