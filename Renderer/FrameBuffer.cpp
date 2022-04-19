#include "FrameBuffer.h"
#include <iostream>

FrameBuffer* FrameBuffer::Init(Vector2Int size, GLenum colourInternalType, GLenum depthInternalType, bool useDepth)
{
	if (loaded)
		throw "FrameBuffer already loaded";

	colour = Texture2D::InitNewEmpty(size, colourInternalType, 0, TextureFiltering::Linear, TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode::BorderClamp);
	if (useDepth)
		depth = Texture2D::InitNewEmpty(size, depthInternalType, 0, TextureFiltering::Linear, TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode::BorderClamp);
	
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour->GetID(), 0);
	if (useDepth)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->GetID(), 0);
	
	GLenum st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (st != GL_FRAMEBUFFER_COMPLETE)
		throw "framebuffer failed to load";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	loaded = true;
	ownsTextures = true;
	return this;
}

FrameBuffer* FrameBuffer::Init(Texture2D* colourTexture, Texture2D* depthTexture)
{
	if (loaded)
		throw "FrameBuffer already loaded";

	if (depthTexture && colourTexture->GetSize() != depthTexture->GetSize())
		throw "textures do not match";
	
	depth = depthTexture;
	colour = colourTexture;

	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour->GetID(), 0);
	if (depth)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->GetID(), 0);
	GLenum st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (st != GL_FRAMEBUFFER_COMPLETE)
		throw "framebuffer failed to load";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ownsTextures = false;
	loaded = true;
	return this;
}

FrameBuffer* FrameBuffer::InitNew(Vector2Int size, GLenum colourInternalType, GLenum depthInternalType, bool useDepth)
{
	FrameBuffer* buf = new FrameBuffer();
	buf->Init(size, colourInternalType, depthInternalType, useDepth);
	return buf;
}

FrameBuffer* FrameBuffer::InitNew(Texture2D* colourTexture, Texture2D* depthTexture)
{
	FrameBuffer* buf = new FrameBuffer();
	buf->Init(colourTexture, depthTexture);
	return buf;
}

void FrameBuffer::Resize(Vector2Int newSize)
{
	std::cout << glGetError() << std::endl;
	colour->Resize(newSize);
	std::cout << glGetError() << std::endl;
	depth->Resize(newSize);
	std::cout << glGetError() << std::endl;
}

void FrameBuffer::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::ClearUsed()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
{
	if (other.loaded)
	{
		loaded = other.loaded;
		colour = other.colour;
		depth = other.depth;
		id = other.id;
		ownsTextures = other.ownsTextures;
	}
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
{
	if (loaded)
	{
		glDeleteFramebuffers(1, &id);
		loaded = false;

		if (ownsTextures)
		{
			delete colour;
			colour = nullptr;
			delete depth;
			depth = nullptr;
		}
	}
	loaded = other.loaded;
	colour = std::move(other.colour);
	depth = std::move(other.depth);
	id = other.id;
	ownsTextures = other.ownsTextures;
	return *this;
}

FrameBuffer::~FrameBuffer()
{
	if (loaded)
	{
		glDeleteFramebuffers(1, &id);
		loaded = false;
		if (ownsTextures)
		{
			delete colour;
			colour = nullptr;
			delete depth;
			depth = nullptr;
		}
	}
}
