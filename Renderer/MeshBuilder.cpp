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
		//aiProcess_GenUVCoords |
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

Object* MeshBuilder::AutoConstructObject(const char* path, Material* litMaterial)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcessPreset_TargetRealtime_Fast);

	if (scene == nullptr) {
		auto* msg = importer.GetErrorString();
		throw std::runtime_error(msg);
	}

	int meshCount = 0;
	MeshData* meshDatas = LoadMesh(meshCount, scene);

	Renderer& r = Program::GetInstance()->GetRenderer();
	//since object can only hold one mesh right now, focus on the first mesh
	Mesh* mesh = r.CreateMesh(meshDatas[0]);
	Material* m = r.AddMaterial(litMaterial->Clone());
	//http://assimp.sourceforge.net/lib_html/materials.html
	// ^ for reference
	/*if (scene->HasTextures())
	{
		int texCount = scene->mNumTextures;
		for (size_t i = 0; i < texCount; i++)
		{
			scene->text
		}
	}*/
	if (scene->HasMaterials()) 
	{
		aiMaterial* material = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex];

		//load properties n stuff into material
		//load textures also

	}
	

	return nullptr;

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
		if (mesh.HasTextureCoords(0))
		{
			//we only care about the first texCoord right now because that's all MeshData supports
			for (size_t j = 0; j  < vCount; j ++)
			{
				aiVector3D vec = mesh.mTextureCoords[0][j];
				//this ignores the last component of the 3D texCoord, cuz we only use 2D texCoords
				mD.texCoords[j] = MeshData::PackTexCoord(Vector2(vec.x, vec.y));
			}
			
		}
		if (mesh.HasVertexColors(0))
		{
			//once again have to loop through stuff
			for (size_t j = 0; j < vCount; j++)
			{
				mD.colours[j] = MeshData::PackColour(*(Vector4*)&(mesh.mColors[0][j]));
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
