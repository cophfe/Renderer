#include "MeshData.h"
#include <stdexcept>

MeshData* MeshData::AllocateMeshData(Size vertexCount, Size indexCount)
{
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	uint32_t size = GetBufferSize();

	void* data = malloc(size);
	if (!data)
		throw std::runtime_error("Failed to allocate memory");

	memset(data, 0, size);

	size_t position = (size_t)data;

	positions = (Position*)position;
	position += sizeof(Position) * vertexCount;

 	normals = (Normal*)position;
	position += sizeof(Normal) * vertexCount;
 	tangents = (Normal*)position;
	position += sizeof(Normal) * vertexCount;
 	bitangents = (Normal*)position;
	position += sizeof(Normal) * vertexCount;
	texCoords = (TexCoord*)position;

	if (indexCount)
		indices = new Index[indexCount];
	else
		indices = nullptr;

	return this;
}

#pragma region Set Data
void MeshData::SetPositions(const Position* positions, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	memcpy(this->positions, positions, count * sizeof(Position));
}

void MeshData::SetNormals(const Normal* normals, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	memcpy(this->normals, normals, count * sizeof(Normal));
}

void MeshData::SetTangents(const Normal* tangents, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	memcpy(this->tangents, tangents, count * sizeof(Normal));
}

void MeshData::SetBitangents(const Normal* bitangents, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	memcpy(this->bitangents, bitangents, count * sizeof(Normal));
}

#ifndef NormalIsVector3
void MeshData::SetNormals(const Vector3* normals, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < vertexCount; i++)
	{
		this->normals[i] = PackNormal(normals[i]);
	}
}
#endif

void MeshData::SetTexCoords(const TexCoord* coords, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";
	
	memcpy(this->texCoords, texCoords, count * sizeof(TexCoord));
}
//
//void MeshData::SetTexCoords(const Vector2* coords, Size count)
//{
//	if (count != vertexCount)
//		throw "Array size does not match vertex count\n";
//
//	for (Size i = 0; i < count; i++)
//	{
//		this->texCoords[i] = PackTexCoord(coords[i]);
//	}
//}

void MeshData::SetVerticesData(const VertexData* vertices, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < vertexCount; i++)
	{
		positions[i] = vertices[i].pos;
#ifdef NormalIsVector3
		normals[i] = vertices[i].normal;
#else
		normals[i] = PackNormal(vertices[i].normal);
#endif
		texCoords[i] = vertices[i].texCoord;// PackTexCoord(vertices[i].texCoord);
	}
}

void MeshData::CalculateNormals(bool clockwise)
{
	float multiplier = clockwise ? -1 : 1;

	for (size_t i = 0; i + 2 < indexCount; i += 3)
	{
		size_t i1 = indices[i];
		size_t i2 = indices[i + 1];
		size_t i3 = indices[i + 2];

		Vector3 normal = glm::cross(positions[i2] - positions[i1], positions[i3] - positions[i1]);
#ifdef NormalIsVector3
		Normal n = normal;
#else
		Normal n = PackNormal(multiplier * glm::normalize(normal));
#endif
		
		normals[i1] = n;
		normals[i2] = n;
		normals[i3] = n;

		//tangent is parallel to the normal, pointed in the direction of the texcoord
		//https://learnopengl.com/Advanced-Lighting/Normal-Mapping

		Vector2 dirUVFirst = texCoords[i2] - texCoords[i1];
		Vector2 dirUVSecond = texCoords[i3] - texCoords[i1];

		Vector3 edgeFirst = positions[i2] - positions[i1];
		Vector3 edgeSecond = positions[i3] - positions[i1];
		//cross.z ^ - 1
		Vector3 t = 1.0f / (dirUVFirst.x * dirUVSecond.y - dirUVSecond.x * dirUVFirst.y) *
			Vector3(
				dirUVSecond.y * edgeFirst.x - dirUVFirst.y * edgeSecond.x,
				dirUVSecond.y * edgeFirst.y - dirUVFirst.y * edgeSecond.y,
				dirUVSecond.y * edgeFirst.z - dirUVFirst.y * edgeSecond.z);
		tangents[i1] = t;
		tangents[i2] = t;
		tangents[i3] = t;

		Vector3 b = glm::cross(n, t);
		bitangents[i1] = b;
		bitangents[i2] = b;
		bitangents[i3] = b;
	}
}

void MeshData::SetIndices(const Index* indices, Size count)
{
	if (count != indexCount)
		throw "Array size does not match index count\n";

	memcpy(this->indices, indices, count * sizeof(Index));
}

#pragma endregion

#pragma region Pack Data
#ifndef NormalIsVector3
MeshData::Normal MeshData::PackNormal(const Vector3& normal)
{
	//formatted as GL_INT_2_10_10_10_REV
	//I had no idea how to do that so thanks to https://stackoverflow.com/questions/35961057/how-to-pack-normals-into-gl-int-2-10-10-10-rev
	//who got it from https://www.gamedev.net/forums/topic/685081-normal-vector-artifacts-with-nvmeshmender/

	const uint32_t xs = normal.x < 0;
	const uint32_t ys = normal.y < 0;
	const uint32_t zs = normal.z < 0;
	Normal packed =
		zs << 29 | ((uint32_t)(normal.z * 511 + (zs << 9)) & 511) << 20 |
		ys << 19 | ((uint32_t)(normal.y * 511 + (ys << 9)) & 511) << 10 |
		xs << 9 | ((uint32_t)(normal.x * 511 + (xs << 9)) & 511);

	return normal;
}
#endif

//MeshData::TexCoord MeshData::PackTexCoord(const Vector2& texcoord)
//{
//	//2x GL_UNSIGNED_SHORT
//	
//	return TexCoord((GLushort)(texcoord.x * USHRT_MAX), (GLushort)(texcoord.y * USHRT_MAX));
//}
#pragma endregion

MeshData::Size MeshData::GetBufferSize()
{
	return vertexCount * (sizeof(Position) + 3 * sizeof(Normal) + sizeof(TexCoord));
}

MeshData::MeshData(MeshData&& other) noexcept
{
	if (other.positions) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;
		
		AllocateMeshData(vertexCount, indexCount);

		positions = other.positions;
		normals = other.normals;
		texCoords = other.texCoords;
		indices = other.indices;

		other.positions = nullptr;
		other.normals = nullptr;
		other.texCoords = nullptr;
		other.indices = nullptr;
	}
	else
	{
		//else do not initiate
		positions = nullptr;
	}

}

MeshData& MeshData::operator=(MeshData&& other) noexcept
{
	if (positions)
		free(positions);
	if (indices)
		delete[] indices;

	if (other.positions) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;

		AllocateMeshData(vertexCount, indexCount);

		positions = other.positions;
		normals = other.normals;
		texCoords = other.texCoords;
		indices = other.indices;

		other.positions = nullptr;
		other.normals = nullptr;
		other.texCoords = nullptr;
		other.indices = nullptr;
	}
	else
	{
		//else do not initiate
		positions = nullptr;
	}
	return *this;
}

MeshData::~MeshData()
{
	if (positions)
	{
		free(positions);
		positions = nullptr;
		normals = nullptr;
		texCoords = nullptr;
	}

	if (indices)
	{
		delete[] indices;
		indices = nullptr;
	}
}

MeshData::MeshData(const MeshData& other)
{
	if (other.positions) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;

		AllocateMeshData(vertexCount, indexCount);
		memcpy(positions, other.positions, GetBufferSize());
		memcpy(indices, other.indices, indexCount * sizeof(Index));
	}
	else
	{
		//else do not initiate
		positions = nullptr;
	}
}

MeshData& MeshData::operator=(const MeshData& other)
{
	if (positions)
		free(positions);
	if (indices)
		delete[] indices;
	
	if (other.positions) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;

		AllocateMeshData(vertexCount, indexCount);
		memcpy(positions, other.positions, GetBufferSize());
		memcpy(indices, other.indices, indexCount * sizeof(Index));
	}
	else
	{
		//else do not initiate
		positions = nullptr;
	}
	return *this;
}
