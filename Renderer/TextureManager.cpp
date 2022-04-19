#include "TextureManager.h"

void TextureManager::Init(const char* texturePath, unsigned int autoMipMapCount,
	TextureFiltering autoTextureFiltering, TextureMipMapFiltering autoMipMapFiltering, TextureWrapMode autoWrapMode)
{
	
	if (path != "" && !std::filesystem::is_directory(path))
	{
		auto msg = "Cannot create texture manager: Path '" + path + "' does not exist or is not a directory";
		throw std::runtime_error(msg);
	}
	path = texturePath;
	
	mipMapCount = autoMipMapCount;
	textureFiltering = autoTextureFiltering;
	mipMapFiltering = autoMipMapFiltering;
	wrapMode = autoWrapMode;
}

Texture2D* TextureManager::LoadTextureParams(const char* name, unsigned int mipMapCount, TextureFiltering textureFiltering, TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode, GLenum internalFormat)
{
	if (!map.contains(name))
	{
		Texture2D* tex = Texture2D::InitNew((path + std::string(name)).c_str(), mipMapCount, textureFiltering, mipMapFiltering, wrapMode, internalFormat);
		map.insert(std::make_pair(std::string(name), tex));
		return tex;
	}
	else
		return map[name];
}

Texture2D* TextureManager::LoadTexture(const char* name)
{
	if (!map.contains(name))
	{
		Texture2D* tex = Texture2D::InitNew((path + std::string(name)).c_str(), mipMapCount, textureFiltering, mipMapFiltering, wrapMode);
		map.insert(std::make_pair(std::string(name), tex));
		return tex;
	}
	else
		return map[name];
}

void TextureManager::Unload()
{
	for (auto& texPair : map)
	{
		delete texPair.second;
	}
	map.clear();
}

TextureManager::~TextureManager()
{
	Unload();
}
