#include "Mesh.h"
#include <iostream>
#include "Program.h"

Mesh* Mesh::Init(MeshData* datas, uint16_t dataCount, bool isStatic, bool storeMeshOnCPU)
{
	if (created)
	{
		auto msg = "Failed to create mesh: Mesh was already created.";
		throw msg;
	}

	submeshCount = dataCount;
	submeshes = new SubMesh[submeshCount];

	for (uint16_t i = 0; i < dataCount; i++)
	{
		InitSubMesh(datas[i], isStatic, i);

		if (storeMeshOnCPU)
			submeshes[i].data = new MeshData(datas[i]);
		else
			submeshes[i].data = nullptr;
	}
	created = true;

	return this;
}

Mesh* Mesh::InitNew(MeshData* datas, uint16_t dataCount, bool isStatic, bool storeMeshOnCPU)
{
	Mesh* mesh = new Mesh();
	mesh->Init(datas, dataCount, isStatic, storeMeshOnCPU);
	//register to program
	auto& r = Program::GetInstance()->GetRenderer();
	if (r.GetAutoRegisterMeshes())
		r.RegisterMesh(mesh);
	return mesh;
}

void Mesh::InitSubMesh(MeshData& data, bool isStatic, uint16_t index)
{
	SubMesh& submesh = submeshes[index];

	submesh.indicesCount = data.indexCount;
	submesh.verticesCount = data.vertexCount;
	//set vertex array
	glGenVertexArrays(1, &submesh.vertexArray);
	glBindVertexArray(submesh.vertexArray);

	//Setup vertex buffer
	glGenBuffers(1, &submesh.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, submesh.vertexBuffer);


	int bufferSize = data.GetBufferSize();
	void* meshBuffer = data.GetBuffer();

	if (!meshBuffer)
	{
		throw "MeshData was empty, mesh could not be created";
	}

	if (isStatic)
		glBufferData(GL_ARRAY_BUFFER, bufferSize, meshBuffer, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, bufferSize, meshBuffer, GL_DYNAMIC_DRAW);

	//setup vertex attributes
	//vertex attributes are stored sequentially in the same buffer
	//Position
	GLuint attribIndex = 0;
	size_t offset = 0;
	if (data.positions)
	{
		glVertexAttribPointer(attribIndex, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Position), (void*)0);
		glEnableVertexAttribArray(attribIndex);
		offset += submesh.verticesCount * sizeof(MeshData::Position);
	}
	++attribIndex;
	//Normal
	if (data.normals)
	{
		glVertexAttribPointer(attribIndex, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Normal), (void*)offset);
		glEnableVertexAttribArray(attribIndex);
		offset += submesh.verticesCount * sizeof(MeshData::Normal);
	}
	++attribIndex;
	if (data.tangents)
	{
		glVertexAttribPointer(attribIndex, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Normal), (void*)offset); //tangent
		glEnableVertexAttribArray(attribIndex);
		offset += submesh.verticesCount * sizeof(MeshData::Normal);
	}
	++attribIndex;
	if (data.bitangents)
	{
		glVertexAttribPointer(attribIndex, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Normal), (void*)offset); //bitangent
		glEnableVertexAttribArray(attribIndex);
		offset += submesh.verticesCount * sizeof(MeshData::Normal);
	}
	++attribIndex;

	//TexCoord
	if (data.texCoords)
	{
		glVertexAttribPointer(attribIndex, 2, GL_FLOAT, GL_FALSE, sizeof(MeshData::TexCoord), (void*)offset);
		glEnableVertexAttribArray(attribIndex);
	}

	//Now set up element buffer
	if (submesh.indicesCount)
	{
		glGenBuffers(1, &submesh.elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.elementBuffer);

		if (isStatic)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, submesh.indicesCount * sizeof(MeshData::Index), data.indices, GL_STATIC_DRAW);
		else
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, submesh.indicesCount * sizeof(MeshData::Index), data.indices, GL_DYNAMIC_DRAW);
	}
}

void Mesh::Render() const
{
	for (uint16_t i = 0; i < submeshCount; i++)
	{
		auto& submesh = submeshes[i];
		submesh.Render();

	}
}

SubMesh* Mesh::GetSubMesh(uint16_t index) const
{
	if (index < 0 || index >= submeshCount)
		return nullptr;

	return &(submeshes[index]);
}

Mesh::Mesh(Mesh&& other) noexcept
{
	created = other.created;
	submeshes = other.submeshes;
	other.submeshes = nullptr;
	other.submeshCount = 0;
	other.created = false;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (created)
	{
		if (submeshes)
		{
			for (uint16_t i = 0; i < submeshCount; i++)
			{
				auto& submesh = submeshes[i];
				glDeleteVertexArrays(1, &submesh.vertexArray);

				//this will break if elementBuffer is not after vertexBuffer
				if (submesh.indicesCount)
					glDeleteBuffers(2, &submesh.vertexBuffer);
				else
					glDeleteBuffers(1, &submesh.vertexBuffer);
			}
			delete[] submeshes;
		}
		submeshes = nullptr;
	}

	created = other.created;
	submeshes = other.submeshes;
	other.submeshes = nullptr;
	other.submeshCount = 0;
	other.created = false;

	return *this;
}

Mesh::~Mesh()
{
	if (created)
	{
		if (submeshes)
		{
			for (uint16_t i = 0; i < submeshCount; i++)
			{
				auto& submesh = submeshes[i];
				glDeleteVertexArrays(1, &submesh.vertexArray);

				//this will break if elementBuffer is not after vertexBuffer
				if (submesh.indicesCount)
					glDeleteBuffers(2, &submesh.vertexBuffer);
				else
					glDeleteBuffers(1, &submesh.vertexBuffer);
			}
			delete[] submeshes;
		}
		submeshes = nullptr;
		created = false;
	}
}