#include "MeshData.h"

MeshData* MeshData::AllocateMeshData(Size vertexCount, Size indexCount)
{
	uint32_t size = vertexCount * (3 * sizeof(Vector3) + sizeof(Vector2));
	void* data = malloc(size);
	memset(data, 0, size);

	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	size_t position = (size_t)data;

	positions = (Position*)position;
	position += sizeof(Position) * vertexCount;

 	normals = (Normal*)position;
	position += sizeof(Normal) * vertexCount;

	colours = (Colour*)position;
	position += sizeof(Colour) * vertexCount;

	texCoords = (TexCoord*)position;

	if (indexCount)
		indices = new Index[indexCount];
	else
		indices = nullptr;

	return this;
}

#pragma region Set Data
void MeshData::SetPositions(Position* positions, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	memcpy(this->positions, positions, count * sizeof(Position));
}

void MeshData::SetNormals(Normal* normals, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	memcpy(this->normals, normals, count * sizeof(Normal));
}

void MeshData::SetNormals(Vector3* normals, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < vertexCount; i++)
	{
		this->normals[i] = PackNormal(normals[i]);
	}
}

void MeshData::SetColours(Colour* colours, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	memcpy(this->colours, colours, count * sizeof(Colour));
}

void MeshData::SetColours(Vector4* colours, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < vertexCount; i++)
	{
		this->colours[i] = PackColour(colours[i]);
	}
}

void MeshData::SetTexCoords(TexCoord* coords, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";
	
	memcpy(this->texCoords, texCoords, count * sizeof(TexCoord));
}

void MeshData::SetTexCoords(Vector2* coords, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < count; i++)
	{
		this->texCoords[i] = PackTexCoord(coords[i]);
	}
}

void MeshData::SetVerticesData(VertexData* vertices, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < vertexCount; i++)
	{
		positions[i] = vertices[i].pos;
		texCoords[i] = PackTexCoord(vertices[i].texCoord);
		colours[i] = PackColour(vertices[i].colour);
		normals[i] = PackNormal(vertices[i].normal);
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
		Normal n = PackNormal(multiplier * glm::normalize(normal));
		
		normals[i1] = n;
		normals[i2] = n;
		normals[i3] = n;
	}
}

void MeshData::SetIndices(Index* indices, Size count)
{
	if (count != indexCount)
		throw "Array size does not match index count\n";

	memcpy(this->indices, indices, count * sizeof(Index));
}

#pragma endregion

#pragma region Pack Data
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

	return packed;
}

MeshData::Colour MeshData::PackColour(const Vector4& colour)
{
	//4x GL_UNSIGNED_BYTE
	return Colour((GLubyte)(colour.x * 255), (GLubyte)(colour.y * 255), (GLubyte)(colour.z * 255), (GLubyte)(colour.w * 255));
}

MeshData::TexCoord MeshData::PackTexCoord(const Vector2 texcoord)
{
	//2x GL_SHORT
	return TexCoord((GLshort)(texcoord.x * 256), (GLshort)(texcoord.y * 256));
}
#pragma endregion

MeshData::Size MeshData::GetBufferSize()
{
	return vertexCount * (sizeof(Position) + sizeof(Normal) + sizeof(Colour) + sizeof(TexCoord));
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
		colours = other.colours;
		texCoords = other.texCoords;
		indices = other.indices;

		other.positions = nullptr;
		other.normals = nullptr;
		other.colours = nullptr;
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
		colours = other.colours;
		texCoords = other.texCoords;
		indices = other.indices;

		other.positions = nullptr;
		other.normals = nullptr;
		other.colours = nullptr;
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
		colours = nullptr;
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
