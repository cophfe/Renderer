#include "Texture.h"

Texture2D* Texture2D::Init(const char* path, TextureFormat format,
	unsigned int mipMapCount, TextureFiltering textureFiltering,
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode)
{
	int channelCount;
	switch (format)
	{
	case TextureFormat::R:
		channelCount = 1;
		break;
	case TextureFormat::RG:
		channelCount = 2;
		break;
	case TextureFormat::RGB:
		channelCount = 3;
		break;
	case TextureFormat::RGBA:
		channelCount = 4;
		break;
	case TextureFormat::Auto:
		channelCount = 0;
		break;
	default:
		throw "Invalid format";
		break;
	}

	int channels;
	unsigned char* data = stbi_load(path, &size.x, &size.y, &channels, channelCount);

	if (!data)
	{
		throw "Image data failed to load";
	}

	GLenum format;
	if (channelCount == 0)
	{
		channelCount = channels;
		switch (channelCount)
		{
		case 1:
			format = TextureFormat::R;
			break;
		case 2:
			format = TextureFormat::RG;
			break;
		case 3:
			format = TextureFormat::RGB;
			break;
		case 4:
			format = TextureFormat::RGBA;
			break;
		default:
			throw "Cannot get format";
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, mipMapCount, channelCount, size.x, size.y, 0, (GLenum)format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	SetFiltering(filtering);
	SetWrapMode(wrapMode);
	SetFiltering(mipMapFiltering);

	stbi_image_free(data);

	this->format = format;
	loaded = true;
	return this;
}

Texture2D* Texture2D::InitNew(const char* path, TextureFormat format,
	unsigned int mipMapCount, TextureFiltering textureFiltering,
	TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode)
{
	Texture2D* tex = new Texture2D();
	tex->Init(path, format, mipMapCount);
	return tex;
}

void Texture2D::SetWrapMode(TextureWrapMode wrapMode)
{
	this->wrapMode = wrapMode;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, (GLenum)wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapMode);
}

void Texture2D::SetFiltering(TextureFiltering filtering)
{
	this->filtering = filtering;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_)
}

void Texture2D::SetFiltering(TextureMipMapFiltering filtering)
{
	mipMapFiltering = filtering;
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
}

Texture2D::~Texture2D()
{
	if (loaded)
	{
		glDeleteTextures(1, &id);
		loaded = false;
	}
}
