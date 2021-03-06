#pragma once
#include "Math.h"
#include "Graphics.h"

//Made this define because for some goddamn reason using a normal stored as GL_INT_2_10_10_10_REV in the fragment shader means I can't sample a texture (even if the thing the normal is doing never actually contributes to anything). 
//temporarily have vector3 stuff until it is figured out

enum MeshContentFlags
{
	MESH_POSITIONS = 0b1,
	MESH_NORMALS = 0b10,
	MESH_TANGENTS = 0b100,
	MESH_BITANGENTS = 0b1000,
	MESH_TEXCOORDS = 0b10000,
};

//used to create a mesh
struct MeshData
{
	typedef glm::vec<3, float> Position;
	typedef glm::vec<3, float> Normal;

	typedef glm::vec<2, float> TexCoord;

	typedef unsigned int Index;
	typedef unsigned int Size;

	struct VertexData 
	{
		VertexData() = default;
		VertexData(Vector3 pos, Vector2 texCoord, Vector3 normal = Vector3()) : pos(pos), normal(normal), texCoord(texCoord) {}

		Vector3 pos;
		Vector3 normal;
		Vector2 texCoord;
	};

	MeshData* AllocateMeshData(Size vertexCount, Size indexCount, GLushort contentFlags = 0xFFFF);

	void SetPositions(const Position* positions, Size count);
	void SetNormals(const Normal* normals, Size count);
	void SetTangents(const Normal* tangents, Size count);
	void SetBitangents(const Normal* biTangents, Size count);
	void SetTexCoords(const TexCoord* coords, Size count);
	void SetIndices(const Index* indices, Size count);
	void SetVerticesData(const VertexData* vertices, Size count);
	
	void CalculateNormalTangentBitangents();
	GLushort GetContentFlags() const { return flags; }
	//static TexCoord PackTexCoord(const Vector2& texcoord);

	//https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
	//https://anteru.net/blog/2016/storing-vertex-data-to-interleave-or-not-to-interleave/
	//that is the question
	//it basically says de-interleaved is good, even on the cpu, so imma just trust it and store stuff seperately 
	Index* indices;

	//these are stored sequentially in memory, just for ezness factor (aka this is the buffer that will be uploaded onto the gpu)
	Position* positions;
	Normal* normals;
	Normal* tangents;
	Normal* bitangents;
	TexCoord* texCoords;
	GLushort flags;
	void* data;

	Size indexCount;
	Size vertexCount;
	
	inline void* GetBuffer() { return data; }
	Size GetBufferSize();

	MeshData() { positions = nullptr; }
	MeshData(MeshData&& other) noexcept;
	MeshData& operator= (MeshData&& other) noexcept;
	~MeshData();
	MeshData(const MeshData& other);
	MeshData& operator= (const MeshData& other);

private:
};

