#include "UniformBuffer.h"

GLuint UniformBuffer::blockBindingTotalCount = 0;
GLuint UniformBuffer::currentUniformBuffer = -1;

void UniformBuffer::Init(int bufferSize, bool dynamic, GLint explicitBinding)
{
	if (created)
	{
		throw "Uniform buffer cannot be initiated more than once";
	}

	glGenBuffers(1, &id);
	glBindBuffer(GL_UNIFORM_BUFFER, id);

	GLenum type = dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW; //GL_STREAM_DRAW because it probbbbably will get changed a lot
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, type);

	//Note that in the future I can use glBindBufferRange and have a single large uniform buffer containing everything, accessed as if it was multiple uniform buffers
	if (explicitBinding < 0)
	{
		blockBinding = blockBindingTotalCount;
		blockBindingTotalCount++;
		glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, id);
	}
	else 
	{
		blockBindingTotalCount = explicitBinding < blockBindingTotalCount ? blockBindingTotalCount + 1 : explicitBinding + 1;
		blockBinding = explicitBinding;
		glBindBufferBase(GL_UNIFORM_BUFFER, explicitBinding, id);
	}

	size = bufferSize;
	created = true;
	currentUniformBuffer = -1;
}

void UniformBuffer::Use() const
{
	if (id != currentUniformBuffer)
	{
		currentUniformBuffer = id;
		glBindBuffer(GL_UNIFORM_BUFFER, id);
	}
}

void UniformBuffer::BindToMaterial(GLuint shaderID, const char* uniformBlockName) const
{
	GLuint index = glGetUniformBlockIndex(shaderID, uniformBlockName);
	BindToMaterial(shaderID, index);
}

void UniformBuffer::BindToMaterial(GLuint shaderID, GLuint uniformBlockIndex) const
{
	glUniformBlockBinding(shaderID, uniformBlockIndex, blockBinding);
}

void UniformBuffer::BufferSubData(GLuint offset, GLuint size, const void* data)
{
	Use();
	if (offset + size > this->size)
	{
		throw "Uniform buffer access out of range";
	}

	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
{
	created = other.created;
	id = other.id;
	blockBinding = other.blockBinding;
	size = other.size;

	other.created = false;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{
	if (created)
	{
		glDeleteShader(id);
	}

	created = other.created;
	id = other.id;
	blockBinding = other.blockBinding;
	size = other.size;

	other.created = false;

	return *this;
}

UniformBuffer::~UniformBuffer()
{
	if (created)
	{
		glDeleteShader(id);
		created = false;
	}
}
