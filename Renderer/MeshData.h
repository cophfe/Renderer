#pragma once
#include "Math.h"
#include "Graphics.h"

//used to create a mesh
struct MeshData
{
	typedef glm::vec<3, float> Position;
	typedef uint32_t Normal;
	struct Colour 
	{
		Colour(GLubyte r, GLubyte g, GLubyte b, GLubyte a) : r(r), g(g), b(b), a(a) {}

		GLubyte r, g, b, a;
	};

	struct TexCoord 
	{
		TexCoord(GLshort x, GLshort y) : x(x), y(y) {}

		GLshort x, y;
	};

	typedef unsigned int Index;
	typedef unsigned int Size;

	struct VertexData 
	{
		VertexData() = default;
		VertexData(Vector3 pos, Vector2 texCoord, Vector3 normal = Vector3(), Vector4 colour = Vector4()) : pos(pos), normal(normal), colour(colour), texCoord(texCoord) {}

		Vector3 pos;
		Vector3 normal;
		Vector4 colour;
		Vector2 texCoord;
	};

	MeshData* AllocateMeshData(Size vertexCount, Size indexCount);

	void SetPositions(Position* positions, Size count);
	void SetNormals(Normal* normals, Size count);
	void SetNormals(Vector3* normals, Size count);
	void SetColours(Colour* colours, Size count);
	void SetColours(Vector4* colours, Size count);
	void SetTexCoords(TexCoord* coords, Size count);
	void SetTexCoords(Vector2* coords, Size count);
	void SetIndices(Index* indices, Size count);
	void SetVerticesData(VertexData* vertices, Size count);
	
	void CalculateNormals(bool clockwise);

	static Normal PackNormal(const Vector3& normal);
	static Colour PackColour(const Vector4& colour);
	static TexCoord PackTexCoord(const Vector2 texcoord);

	//https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
	//https://anteru.net/blog/2016/storing-vertex-data-to-interleave-or-not-to-interleave/
	//that is the question
	//it basically says de-interleaved is good, even on the cpu, so imma just trust it and store stuff seperately 
	Index* indices;

	//these are stored sequentially in memory, just for ezness factor (aka this is the buffer that will be uploaded onto the gpu)
	Position* positions;
	Normal* normals;
	Colour* colours;
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

