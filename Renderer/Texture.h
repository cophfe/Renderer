#pragma once
#include "Graphics.h"
#include "Math.h"

enum class TextureFiltering : GLenum //GL_TEXTURE_MAG_FILTER
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR
};

enum class TextureMipMapFiltering : GLenum //GL_TEXTURE_MIN_FILTER
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR,
	NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
	NearestMipMapLinear = GL_NEAREST_MIPMAP_LINEAR,
	LinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
	LinearMipMapLinear = GL_LINEAR_MIPMAP_LINEAR,
};

enum class TextureWrapMode : GLenum //GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R
{
	Wrap = GL_REPEAT,
	MirrorWrap = GL_MIRRORED_REPEAT,
	BorderClamp = GL_CLAMP_TO_BORDER,
	EdgeClamp = GL_CLAMP_TO_EDGE,
	MirrorClamp = GL_MIRROR_CLAMP_TO_EDGE,
};

enum class TextureFormat : GLenum
{
	R = GL_RED,
	RG = GL_RG,
	RGB = GL_RGB,
	RGBA = GL_RGBA,
	Auto
};

class Texture2D
{
public:
	Texture2D() { loaded = false; }
	Texture2D* Init(const char* path, TextureFiltering textureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering mipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode wrapMode = TextureWrapMode::Wrap, GLenum internalFormat = GL_RGBA);
	Texture2D* InitEmpty(Vector2Int size, GLenum internalFormat = GL_RGBA, TextureFiltering textureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering mipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode wrapMode = TextureWrapMode::Wrap);

	static Texture2D* InitNew(const char* path, TextureFiltering textureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering mipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode wrapMode = TextureWrapMode::Wrap, GLenum internalFormat = GL_RGBA);
	static Texture2D* InitNewEmpty(Vector2Int size, GLenum internalFormat = GL_RGBA, TextureFiltering textureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering mipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode wrapMode = TextureWrapMode::Wrap);

	void SetWrapMode(TextureWrapMode wrapMode);
	void SetFiltering(TextureFiltering filtering);
	void SetFiltering(TextureMipMapFiltering filtering);
	void Resize(Vector2Int newSize);

	inline GLuint GetID() const { return id; }
	inline Vector2Int GetSize() const { return size; }
	
	Texture2D(Texture2D&& other) noexcept;
	Texture2D& operator= (Texture2D&& other) noexcept;
	~Texture2D();
	Texture2D(const Texture2D& other) = delete;
	Texture2D& operator= (const Texture2D& other) = delete;
private:
	void DebugDraw(unsigned char* data);

	GLuint id;
	Vector2Int size;

	bool hasMipMaps = false;
	GLenum internalFormat;

	TextureFiltering filtering;
	TextureMipMapFiltering mipMapFiltering;
	TextureWrapMode wrapMode;

	bool loaded;
};

