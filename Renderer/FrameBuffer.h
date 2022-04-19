#pragma once
#include "Graphics.h"
#include "Math.h"
#include "Texture.h"
class FrameBuffer
{
public:
	FrameBuffer* Init(Vector2Int size, GLenum colourInternalType = GL_RGBA, GLenum depthInternalType = GL_DEPTH_COMPONENT, bool useDepth = true);
	FrameBuffer* Init(Texture2D* colourTexture, Texture2D* depthTexture = nullptr);
	static FrameBuffer* InitNew(Vector2Int size, GLenum colourInternalType = GL_RGBA, GLenum depthInternalType = GL_DEPTH_COMPONENT, bool useDepth = true);
	static FrameBuffer* InitNew(Texture2D* colourTexture, Texture2D* depthTexture = nullptr);
	void Resize(Vector2Int newSize);
	void Use();
	static void ClearUsed();

	Texture2D* GetColour() { return colour; }
	Texture2D* GetDepth() { return depth; }
	//these are used if you want to continue using a texture, but delete a framebuffer. this is a jank workaround, i know :(
	Texture2D* PopColour() { Texture2D* c = colour; colour = nullptr; return c; }
	Texture2D* PopDepth() { Texture2D* d = depth; depth = nullptr; return d; }

	GLuint GetID() const { return id; }
	
	FrameBuffer() = default;
	FrameBuffer(FrameBuffer&& other) noexcept;
	FrameBuffer& operator= (FrameBuffer&& other) noexcept;
	~FrameBuffer();
	FrameBuffer(const FrameBuffer& other) = delete;
	FrameBuffer& operator= (const FrameBuffer& other) = delete;
private:
	Texture2D* colour;
	Texture2D* depth;
	bool ownsTextures;
	GLuint id;
	bool loaded = false;
};

