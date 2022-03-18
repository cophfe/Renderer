#pragma once
#include <vector>
#include "MeshData.h"
#include "Graphics.h"

class Mesh
{
public:
	Mesh() { created = false; }

	void Init(MeshData& data, bool isStatic = true, bool storeMeshOnCPU = false); //will clone data if is dynamic
	void Render() const; //shader program and transform should be set up before this is called. 

	inline MeshData* GetStoredMeshData() { return data; } //cam be null
	
	Mesh(Mesh&& other) noexcept;
	Mesh& operator= (Mesh&& other) noexcept;
	~Mesh();
	Mesh(const Mesh& other) = delete;
	Mesh& operator= (const Mesh& other) = delete;

private:
	GLuint vertexArray;
	//vertex info is stored in one big array, but is not interleaved. this is not good for dynamic meshes
	GLuint vertexBuffer; 
	GLuint elementBuffer;

	//I'm not sure if I should even keep this in cpu memory or not??
	//for dynamic meshes it may be useful, but also maybe not. for static meshes it is straight up useless, i assume. madre de dios este es muy confuso, no estoy inteligente
	MeshData* data;
	uint32_t verticesCount;
	uint32_t indicesCount;
	
	bool created;
};

