#include "MeshData.h"

MeshData&& MeshData::AllocateMeshData(Size vertexCount, Size indexCount)
{
	uint32_t size = vertexCount * (3 * sizeof(Vector3) + sizeof(Vector2));
	void* data = malloc(size);
	MeshData mesh;
	
	int position = 0;

	mesh.positions = (Position*)data;
	position += sizeof(Position) * vertexCount;

	mesh.normals = (Normal*)((uint32_t)data + position);
	position += sizeof(Normal) * vertexCount;

	mesh.colours = (Colour*)((uint32_t)data + position);
	position += sizeof(Colour) * vertexCount;

	mesh.texCoords = (TexCoord*)((uint32_t)data + position);

	if (indexCount)
		mesh.indices = new Index[indexCount];
	else
		mesh.indices = nullptr;

	return std::move(mesh);
}

void MeshData::SetPositions(Position* positions, Size count, bool clone)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	if (clone)
	{
		memcpy(this->positions, positions, count * sizeof(Position));
	}
	else
	{
		this->positions = positions;
	}
}

void MeshData::SetNormals(Normal* normals, Size count, bool clone)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	if (clone)
	{
		memcpy(this->normals, normals, count * sizeof(Normal));
	}
	else
	{
		this->normals = normals;
	}
}

void MeshData::SetNormals(Vector3* normals, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < count; i++)
	{
		this->normals[i] = PackNormal(normals[i]);
	}
}

void MeshData::SetColours(Colour* colours, Size count, bool clone)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	if (clone)
	{
		memcpy(this->colours, colours, count * sizeof(Colour));
	}
	else
	{
		this->colours = colours;
	}
}

void MeshData::SetColours(Vector4* colours, Size count)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";

	for (Size i = 0; i < count; i++)
	{
		this->colours[i] = PackColour(colours[i]);
	}
}

void MeshData::SetTexCoords(TexCoord* coords, Size count, bool clone)
{
	if (count != vertexCount)
		throw "Array size does not match vertex count\n";
	
	if (clone)
	{
		memcpy(this->texCoords, texCoords, count * sizeof(TexCoord));
	}
	else
	{
		this->texCoords = coords;
	}
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
	return Colour((GLubyte)(colour.x * 256), (GLubyte)(colour.y * 256), (GLubyte)(colour.z * 256), (GLubyte)(colour.w * 256));
}

MeshData::TexCoord MeshData::PackTexCoord(const Vector2 texcoord)
{
	//2x GL_SHORT
	return TexCoord((GLshort)(texcoord.x * 256), (GLshort)(texcoord.y * 256));
}

void MeshData::SetIndices(Index* indices, Size count, bool clone)
{
	if (count != indexCount)
		throw "Array size does not match index count\n";

	if (clone)
	{
		memcpy(this->indices, indices, count * sizeof(Index));
	}
	else
	{
		this->indices = indices;
	}
}

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
