#pragma once
#include "Graphics.h"
#include <stdexcept>

class UniformBuffer
{
public:
	void Init(int bufferSize, bool dynamic = true, GLint explicitBinding = -1);
	void Use() const;
	void BindToMaterial(GLuint shaderID, const char* uniformBlockName) const;
	void BindToMaterial(GLuint shaderID, GLuint uniformBlockIndex) const;
	void BufferSubData(GLuint offset, GLuint size, const void* data);

	inline unsigned int GetSize() const { return size; }
	inline GLuint GetId() const { return id; }
	inline GLuint GetBlockBinding() const { return blockBinding; }

	UniformBuffer() { created = false; }
	UniformBuffer(UniformBuffer&& other) noexcept;
	UniformBuffer& operator= (UniformBuffer&& other) noexcept;
	~UniformBuffer();
	UniformBuffer(const UniformBuffer& other) = delete;
	UniformBuffer& operator= (const UniformBuffer& other) = delete;

private:
	static GLuint currentUniformBuffer;
	static GLuint blockBindingTotalCount;

	GLuint blockBinding;
	GLuint id;
	unsigned int size;
	bool created;
};

