#include "TextureManager.h"

void TextureManager::Init(const char* texturePath,
	TextureFiltering autoTextureFiltering, TextureMipMapFiltering autoMipMapFiltering, TextureWrapMode autoWrapMode, GLenum autoInternalFormat)
{
	
	if (path != "" && !std::filesystem::is_directory(path))
	{
		auto msg = "Cannot create texture manager: Path '" + path + "' does not exist or is not a directory";
		throw std::runtime_error(msg);
	}
	path = texturePath;
	
	textureFiltering = autoTextureFiltering;
	mipMapFiltering = autoMipMapFiltering;
	wrapMode = autoWrapMode;
	internalFormat = autoInternalFormat;

	defaultTexture = LoadTexture("white.png");
	if (defaultTexture == nullptr)
	{
		ownsDefault = true;
		defaultTexture = Texture2D::InitNewEmpty(Vector2Int(1, 1));
	}
}

Texture2D* TextureManager::LoadTextureParams(const char* name, TextureFiltering textureFiltering, TextureMipMapFiltering mipMapFiltering, TextureWrapMode wrapMode, GLenum internalFormat)
{
	if (!map.contains(name))
	{
		Texture2D* tex = Texture2D::InitNew((path + std::string(name)).c_str(), textureFiltering, mipMapFiltering, wrapMode, internalFormat);
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
		Texture2D* tex = Texture2D::InitNew((path + std::string(name)).c_str(), textureFiltering, mipMapFiltering, wrapMode);
		if (tex == nullptr)
		{
			return defaultTexture;

		}
		map.insert(std::make_pair(std::string(name), tex));
		return tex;
	}
	else
		return map[name];
}

void TextureManager::Unload()
{
	if (ownsDefault)
	{
		delete defaultTexture;
		defaultTexture = nullptr;
	}
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
