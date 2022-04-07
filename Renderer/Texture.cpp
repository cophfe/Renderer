#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <sstream>

Texture2D* Texture2D::Init(const char* path,
	unsigned int mipMapCount, TextureFiltering textureFiltering,
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode)
{
	int channels = 0;
	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &size.x, &size.y, &channels, 4);

	if (data == nullptr)
	{
		throw "Image data failed to load";
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, mipMapCount, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	SetFiltering(textureFiltering);
	SetWrapMode(wrapMode);
	SetFiltering(mipMapFiltering);

	//DebugDraw(data);

	stbi_image_free(data);

	this->format = format;
	loaded = true;
	return this;
}

Texture2D* Texture2D::InitNew(const char* path,
	unsigned int mipMapCount, TextureFiltering textureFiltering,
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode)
{
	Texture2D* tex = new Texture2D();
	tex->Init(path, mipMapCount, textureFiltering, mipMapFiltering, wrapMode);
	return tex;
}

void Texture2D::SetWrapMode(TextureWrapMode wrapMode)
{
	glBindTexture(GL_TEXTURE_2D, id);

	this->wrapMode = wrapMode;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)filtering);
}

Texture2D::Texture2D(Texture2D&& other) noexcept
{
	id = other.id;
	loaded = other.loaded;
	size = other.size;
	format = other.format;
	filtering = other.filtering;
	mipMapFiltering = other.mipMapFiltering;
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
	format = other.format;
	filtering = other.filtering;
	mipMapFiltering = other.mipMapFiltering;
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
