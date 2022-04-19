#pragma once
#include "Texture.h"
#include "Graphics.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <filesystem>

class TextureManager
{
public:
	void Init(const char* texturePath, unsigned int autoMipMapCount = 0, TextureFiltering autoTextureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering autoMipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode autoWrapMode = TextureWrapMode::Wrap);
	Texture2D* LoadTextureParams(const char* name, unsigned int mipMapCount = 0, TextureFiltering textureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering mipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode wrapMode = TextureWrapMode::Wrap, GLenum internalFormat = GL_RGBA);
	Texture2D* LoadTexture(const char* name);
	
	void Unload();
	
	TextureManager() = default;
	~TextureManager();
	TextureManager(const TextureManager& other) = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
private:
	std::string path;
	std::unordered_map<std::string, Texture2D*> map;

	//auto texture settings
	unsigned int mipMapCount;
	TextureFiltering textureFiltering;
	TextureMipMapFiltering mipMapFiltering;
	TextureWrapMode wrapMode;
};

