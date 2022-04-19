#include "MeshData.h"
#include <stdexcept>

MeshData* MeshData::AllocateMeshData(Size vertexCount, Size indexCount, GLushort contentFlags)
{
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	flags = contentFlags;
	uint32_t size = GetBufferSize();

	data = malloc(size);
	if (!data)
		throw std::runtime_error("Failed to allocate memory");

	memset(data, 0, size);

	size_t position = (size_t)data;

	if (flags & MESH_POSITIONS)
	{
		positions = (Position*)position;
		position += sizeof(Position) * vertexCount;
	}
	else positions = nullptr;

	if (flags & MESH_NORMALS)
	{
		normals = (Normal*)position;
		position += sizeof(Normal) * vertexCount;
	}
	else normals = nullptr;

	if (flags & MESH_TANGENTS)
	{
		tangents = (Normal*)position;
		position += sizeof(Normal) * vertexCount;
	}
	else tangents = nullptr;

	if (flags & MESH_BITANGENTS)
	{
		bitangents = (Normal*)position;
		position += sizeof(Normal) * vertexCount;
	}
	else bitangents = nullptr;

	if (flags & MESH_TEXCOORDS)
	{
		texCoords = (TexCoord*)position;
		position += sizeof(TexCoord) * vertexCount;
	}
	else texCoords = nullptr;

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
	else if (!this->positions)
		throw "MeshData does not contain positions\n";

	memcpy(this->positions, positions, count * sizeof(Position));
}

void MeshData::SetNormals(const Normal* normals, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";
	else if (!this->normals)
		throw "MeshData does not contain normals\n";

	memcpy(this->normals, normals, count * sizeof(Normal));
}

void MeshData::SetTangents(const Normal* tangents, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";
	else if (!this->tangents)
		throw "MeshData does not contain tangents\n";

	memcpy(this->tangents, tangents, count * sizeof(Normal));
}

void MeshData::SetBitangents(const Normal* bitangents, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";
	else if (!this->bitangents)
		throw "MeshData does not contain bitangents\n";

	memcpy(this->bitangents, bitangents, count * sizeof(Normal));
}

void MeshData::SetTexCoords(const TexCoord* coords, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";
	else if (!this->texCoords)
		throw "MeshData does not contain texcoords\n";

	memcpy(this->texCoords, coords, count * sizeof(TexCoord));
}

void MeshData::SetVerticesData(const VertexData* vertices, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < vertexCount; i++)
	{
		if (positions)
			positions[i] = vertices[i].pos;
		if (normals)
			normals[i] = vertices[i].normal;
		if (texCoords)
			texCoords[i] = vertices[i].texCoord;
	}
}

void MeshData::CalculateNormalTangentBitangents()
{

	if (!normals || !tangents || !bitangents)
		return;

	for (size_t i = 0; i + 2 < indexCount; i += 3)
	{
		size_t i1 = indices[i];
		size_t i2 = indices[i + 1];
		size_t i3 = indices[i + 2];

		Vector3 normal = glm::cross(positions[i2] - positions[i1], positions[i3] - positions[i1]);
		Normal n = normal;
		
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
		float uvMod = 1.0f / (dirUVFirst.x * dirUVSecond.y - dirUVSecond.x * dirUVFirst.y);
		Vector3 t = uvMod *
			Vector3(	dirUVSecond.y * edgeFirst.x - dirUVFirst.y * edgeSecond.x,
						dirUVSecond.y * edgeFirst.y - dirUVFirst.y * edgeSecond.y,
						dirUVSecond.y * edgeFirst.z - dirUVFirst.y * edgeSecond.z);
		tangents[i1] = t;
		tangents[i2] = t;
		tangents[i3] = t;

		//the cross bitangent is incorrect in the case of distorted uvs
		//Vector3 b = glm::cross(n, t);
		Vector3 b = uvMod *
			Vector3(-dirUVSecond.x * edgeFirst.x + dirUVFirst.x * edgeSecond.x,
					-dirUVSecond.x * edgeFirst.y + dirUVFirst.x * edgeSecond.y,
					-dirUVSecond.x * edgeFirst.z + dirUVFirst.x * edgeSecond.z);
		
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

MeshData::Size MeshData::GetBufferSize()
{
	return vertexCount * (((flags & MESH_POSITIONS) != 0) * sizeof(Position) + ((flags & MESH_NORMALS) != 0) * sizeof(Normal) + ((flags & MESH_TANGENTS) != 0) * sizeof(Normal) + ((flags & MESH_BITANGENTS) != 0) * sizeof(Normal) + ((flags & MESH_TEXCOORDS) != 0) * sizeof(TexCoord));
}

MeshData::MeshData(MeshData&& other) noexcept
{
	if (other.data) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;
		
		AllocateMeshData(vertexCount, indexCount);

		positions = other.positions;
		normals = other.normals;
		texCoords = other.texCoords;
		indices = other.indices;
		data = other.data;

		other.data = nullptr;
		other.positions = nullptr;
		other.normals = nullptr;
		other.texCoords = nullptr;
		other.tangents = nullptr;
		other.bitangents = nullptr;
		other.indices = nullptr;
	}
	else
	{
		//else do not initiate
		data = nullptr;
	}

}

MeshData& MeshData::operator=(MeshData&& other) noexcept
{
	if (data)
		free(data);
	if (indices)
		delete[] indices;

	if (other.data) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;

		AllocateMeshData(vertexCount, indexCount);

		positions = other.positions;
		normals = other.normals;
		texCoords = other.texCoords;
		tangents = other.tangents;
		bitangents = other.bitangents;
		indices = other.indices;
		data = other.data;

		other.positions = nullptr;
		other.normals = nullptr;
		other.tangents = nullptr;
		other.bitangents = nullptr;
		other.texCoords = nullptr;
		other.indices = nullptr;
		other.data = nullptr;
	}
	else
	{
		//else do not initiate
		data = nullptr;
	}
	return *this;
}

MeshData::~MeshData()
{
	if (data)
	{
		free(data);
		data = nullptr;
		positions = nullptr;
		normals = nullptr;
		texCoords = nullptr;
		tangents = nullptr;
		bitangents = nullptr;
	}

	if (indices)
	{
		delete[] indices;
		indices = nullptr;
	}
}

MeshData::MeshData(const MeshData& other)
{
	if (other.data) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;

		AllocateMeshData(vertexCount, indexCount);
		memcpy(data, other.data, GetBufferSize());
		memcpy(indices, other.indices, indexCount * sizeof(Index));
	}
	else
	{
		//else do not initiate
		data = nullptr;
	}
}

MeshData& MeshData::operator=(const MeshData& other)
{
	if (data)
		free(data);
	if (indices)
		delete[] indices;
	
	if (other.data) //if initiated
	{
		indexCount = other.indexCount;
		vertexCount = other.vertexCount;

		AllocateMeshData(vertexCount, indexCount);
		memcpy(data, other.data, GetBufferSize());
		memcpy(indices, other.indices, indexCount * sizeof(Index));
	}
	else
	{
		//else do not initiate
		data = nullptr;
	}
	return *this;
}
