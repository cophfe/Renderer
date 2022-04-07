#pragma once
#include <vector>
#include "MeshData.h"
#include "Graphics.h"
#include "SubMesh.h"

class Mesh
{
public:
	Mesh() { created = false; }

	Mesh* Init(MeshData* datas, uint16_t dataCount, bool isStatic = true, bool storeMeshOnCPU = false);
	static Mesh* InitNew(MeshData* datas, uint16_t dataCount, bool isStatic = true, bool storeMeshOnCPU = false);
	void Render() const; //shader program and transform should be set up before this is called. 
	
	SubMesh* GetSubMesh(uint16_t index) const;
	inline const SubMesh* GetSubmeshArray() const { return submeshes; }
	inline uint16_t GetSubMeshCount() const { return submeshCount; }
	Mesh(Mesh&& other) noexcept;
	Mesh& operator= (Mesh&& other) noexcept;
	~Mesh();
	Mesh(const Mesh& other) = delete;
	Mesh& operator= (const Mesh& other) = delete;

private:
	void InitSubMesh(MeshData& data, bool isStatic, uint16_t index);
	
	SubMesh* submeshes;
	uint16_t submeshCount;
	bool created;
};

