#pragma once
#include "Math.h"
#include "Graphics.h"

//Made this define because for some goddamn reason using a normal stored as GL_INT_2_10_10_10_REV in the fragment shader means I can't sample a texture (even if the thing the normal is doing never actually contributes to anything). 
//temporarily have vector3 stuff until it is figured out
#define NormalIsVector3 

//used to create a mesh
struct MeshData
{
	typedef glm::vec<3, float> Position;
#ifdef NormalIsVector3
	typedef Vector3 Normal; // uint32_t
#else
	typedef uint32_t Normal;
#endif // NormalIsVector3

	typedef glm::vec<2, float> TexCoord;
	/*struct TexCoord 
	{
		TexCoord(GLushort x, GLushort y) : x(x), y(y) {}

		GLushort x, y;
	};*/

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

	MeshData* AllocateMeshData(Size vertexCount, Size indexCount);

	void SetPositions(const Position* positions, Size count);
	void SetNormals(const Normal* normals, Size count);
#ifndef NormalIsVector3
	void SetNormals(const Vector3* normals, Size count);
#endif
	void SetTexCoords(const TexCoord* coords, Size count);
	//void SetTexCoords(const Vector2* coords, Size count);
	void SetIndices(const Index* indices, Size count);
	void SetVerticesData(const VertexData* vertices, Size count);
	
	void CalculateNormals(bool clockwise);

#ifndef NormalIsVector3
	static Normal PackNormal(const Vector3& normal);
#endif
	//static TexCoord PackTexCoord(const Vector2& texcoord);

	//https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
	//https://anteru.net/blog/2016/storing-vertex-data-to-interleave-or-not-to-interleave/
	//that is the question
	//it basically says de-interleaved is good, even on the cpu, so imma just trust it and store stuff seperately 
	Index* indices;

	//these are stored sequentially in memory, just for ezness factor (aka this is the buffer that will be uploaded onto the gpu)
	Position* positions;
	Normal* normals;
	TexCoord* texCoords;

	Size indexCount;
	Size vertexCount;
	
	inline void* GetBuffer() { return positions; }
	Size GetBufferSize();

	MeshData() { positions = nullptr; }
	MeshData(MeshData&& other) noexcept;
	MeshData& operator= (MeshData&& other) noexcept;
	~MeshData();
	MeshData(const MeshData& other);
	MeshData& operator= (const MeshData& other);
};

