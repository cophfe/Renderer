#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <sstream>

Texture2D* Texture2D::Init(const char* path, TextureFiltering textureFiltering,
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode, GLenum internalFormat)
{	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	int channels = 0;
	GLenum type;
	//stbi_set_flip_vertically_on_load(true);
	union {
		unsigned char* c;
		float* f;
	} data;

	if (internalFormat == GL_RGBA16F || internalFormat == GL_RGBA32F || internalFormat == GL_RGB16F || internalFormat == GL_RGB32F || internalFormat == GL_RG16F || internalFormat == GL_RG32F || internalFormat == GL_R16F || internalFormat == GL_R32F)
	{
		type = GL_FLOAT;
		stbi_ldr_to_hdr_gamma(1.0f); //i wanna do the correction dammit
		data.f = stbi_loadf(path, &size.x, &size.y, &channels, 4);
	}
	else
	{
		type = GL_UNSIGNED_BYTE;
		data.c = stbi_load(path, &size.x, &size.y, &channels, 4);
	}

	if (data.c == nullptr)
	{
		return nullptr;
	}
	
	this->hasMipMaps = mipMapFiltering != TextureMipMapFiltering::Linear && mipMapFiltering != TextureMipMapFiltering::Nearest;
	this->internalFormat = internalFormat;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, GL_RGBA, type, data.c);
	if (hasMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);
	//DebugDraw(data);
	stbi_image_free(data.c);

	SetFiltering(textureFiltering);
	SetWrapMode(wrapMode);
	SetFiltering(mipMapFiltering);
	loaded = true;
	return this;
}

Texture2D* Texture2D::InitEmpty(Vector2Int size, GLenum internalFormat, TextureFiltering textureFiltering, 
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	GLenum format;
	if (internalFormat == GL_DEPTH_COMPONENT || internalFormat == GL_DEPTH_COMPONENT16 || internalFormat == GL_DEPTH_COMPONENT24 || internalFormat == GL_DEPTH_COMPONENT32F)
		format = GL_DEPTH_COMPONENT;
	else
		format = GL_RGBA;

	
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	SetFiltering(textureFiltering);
	SetWrapMode(wrapMode);
	SetFiltering(mipMapFiltering);
	loaded = true;
	this->hasMipMaps = mipMapFiltering != TextureMipMapFiltering::Linear && mipMapFiltering != TextureMipMapFiltering::Nearest;
	this->internalFormat = internalFormat;
	this->size = size;
	return this;
}

Texture2D* Texture2D::InitNew(const char* path, TextureFiltering textureFiltering,
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode, GLenum internalFormat)
{
	Texture2D* tex = new Texture2D();
	
	return tex->Init(path, textureFiltering, mipMapFiltering, wrapMode, internalFormat);
}

Texture2D* Texture2D::InitNewEmpty(Vector2Int size, GLenum internalFormat, TextureFiltering textureFiltering, 
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode)
{ 
	Texture2D* tex = new Texture2D();
	return tex->InitEmpty(size, internalFormat, textureFiltering, mipMapFiltering, wrapMode);
}

void Texture2D::SetWrapMode(TextureWrapMode wrapMode)
{
	glBindTexture(GL_TEXTURE_2D, id);

	this->wrapMode = wrapMode;
	//idk which one of these is z (and I don't need to set), so I'll just set all of em
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, (GLenum)wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapMode);
}

void Texture2D::SetFiltering(TextureFiltering filtering)
{
	glBindTexture(GL_TEXTURE_2D, id);

	this->filtering = filtering;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)filtering);
}

void Texture2D::SetFiltering(TextureMipMapFiltering filtering)
{
	glBindTexture(GL_TEXTURE_2D, id);

	mipMapFiltering = filtering;
	bool newHasMipMaps = mipMapFiltering != TextureMipMapFiltering::Linear && mipMapFiltering != TextureMipMapFiltering::Nearest;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)filtering);
	if (newHasMipMaps == true && hasMipMaps == false)
	{
		hasMipMaps = true;
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

void Texture2D::Resize(Vector2Int newSize)
{
	if (size == newSize)
		return;

	GLenum format;
	if (internalFormat == GL_DEPTH_COMPONENT || internalFormat == GL_DEPTH_COMPONENT16 || internalFormat == GL_DEPTH_COMPONENT24 || internalFormat == GL_DEPTH_COMPONENT32F)
		format = GL_DEPTH_COMPONENT;
	else
		format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	size = newSize;
	
}

Texture2D::Texture2D(Texture2D&& other) noexcept
{
	id = other.id;
	loaded = other.loaded;
	size = other.size;
	hasMipMaps = other.hasMipMaps;
	internalFormat = other.internalFormat;
	filtering = other.filtering;
	wrapMode = other.wrapMode;

	other.loaded = false;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
	if (loaded)
	{
		glDeleteTextures(1, &id);
	}

	id = other.id;
	loaded = other.loaded;
	size = other.size;
	hasMipMaps = other.hasMipMaps;
	internalFormat = other.internalFormat;
	filtering = other.filtering;
	wrapMode = other.wrapMode;

	other.loaded = false;

	return *this;
}

Texture2D::~Texture2D()
{
	if (loaded)
	{
		glDeleteTextures(1, &id);
		loaded = false;
	}
}

void Texture2D::DebugDraw(unsigned char* data)
{
	//only works for UNSIGNED_CHAR datatype
	const std::string shading = " .:-=+*#%@";
	const int xSize = 30;
	int ySize = size.y / size.x * xSize;
	Vector2 sizeMul = Vector2((float)size.x / xSize, (float)size.y / ySize);
	std::stringstream string;

	for (size_t y = 0; y < ySize; y++)
	{
		for (size_t x = 0; x < xSize; x++)
		{
			size_t i = 4 * ((y * sizeMul.y) * size.y + x * sizeMul.x);

			float len = 0.299f * data[i] + 0.587f * data[i + 1] + 0.114f * data[i + 2];
			len = glm::clamp(len / 256, 0.0f, 1.0f);

			string << shading[len * (shading.length() - 1)] << shading[len * (shading.length() - 1)];
		}
		string << "\n";
	}
	std::cout << string.str() << "\n\n";
}
