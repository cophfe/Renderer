#include "Mesh.h"

void Mesh::Init(MeshData& data, bool isStatic, bool storeMeshOnCPU)
{
	indicesCount = data.indexCount;
	verticesCount = data.vertexCount;

	//Setup vertex buffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	int bufferSize = data.GetBufferSize();
	void* meshBuffer = data.GetBuffer();
	
	if (!meshBuffer)
	{
		throw "MeshData was empty, mesh could not be created\n";
	}

	if (isStatic)
		glBufferData(GL_ARRAY_BUFFER, bufferSize, meshBuffer, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, bufferSize, meshBuffer, GL_DYNAMIC_DRAW);

	//setup vertex attributes
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	//vertex attributes are stored sequentially in the same buffer
	//Position
	glVertexAttribPointer(0, verticesCount, GL_FLOAT_VEC3, GL_FALSE, sizeof(MeshData::Position), nullptr);
	glEnableVertexAttribArray(0);
	uint16_t offset = verticesCount * sizeof(MeshData::Position);
	//Normal
	glVertexAttribPointer(1, verticesCount, GL_INT_2_10_10_10_REV, GL_FALSE, sizeof(MeshData::Normal), (void*)offset);
	glEnableVertexAttribArray(1);
	offset += verticesCount * sizeof(MeshData::Normal);
	//Colour
	glVertexAttribPointer(2, verticesCount * 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(MeshData::Colour), (void*)offset);
	glEnableVertexAttribArray(2);
	offset += verticesCount * sizeof(MeshData::Colour);
	//TexCoord
	glVertexAttribPointer(3, verticesCount * 2, GL_SHORT, GL_FALSE, sizeof(MeshData::TexCoord), (void*)offset);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	//Now set up element buffer
	if (indicesCount)
	{
		glGenBuffers(1, &elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

		if (isStatic)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(MeshData::Index), data.indices, GL_STATIC_DRAW);
		else
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(MeshData::Index), data.indices, GL_DYNAMIC_DRAW);
	}

	if (storeMeshOnCPU)
		this->data = new MeshData(data);
	else
		this->data = nullptr;

	created = true;
}

void Mesh::Render() const
{
	glBindVertexArray(vertexArray);
	
	if (indicesCount)
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, GL_TRIANGLES);

	glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other) noexcept
{
	created = other.created;
	vertexArray = other.vertexArray;
	vertexBuffer = other.vertexBuffer;
	elementBuffer = other.elementBuffer;
	verticesCount = other.verticesCount;
	indicesCount = other.indicesCount;
	data = other.data;

	other.created = false;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (created)
	{
		glDeleteVertexArrays(1, &vertexArray);

		//this will break if elementBuffer is not after vertexBuffer
		if (indicesCount)
			glDeleteBuffers(2, &vertexBuffer);
		else
			glDeleteBuffers(1, &vertexBuffer);
	}
	created = other.created;
	vertexArray = other.vertexArray;
	vertexBuffer = other.vertexBuffer;
	elementBuffer = other.elementBuffer;
	verticesCount = other.verticesCount;
	indicesCount = other.indicesCount;
	data = other.data;

	other.created = false;

	return *this;
}

Mesh::~Mesh()
{
	if (created)
	{
		glDeleteVertexArrays(1, &vertexArray);

		//this will break if elementBuffer is not after vertexBuffer
		if (indicesCount)
			glDeleteBuffers(2, &vertexBuffer);
		else
			glDeleteBuffers(1, &vertexBuffer);

		created = false;
	}
}
