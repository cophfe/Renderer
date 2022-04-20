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
	void Init(const char* texturePath, TextureFiltering autoTextureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering autoMipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode autoWrapMode = TextureWrapMode::Wrap, GLenum autoInternalFormat = GL_RGBA);
	Texture2D* LoadTextureParams(const char* name, TextureFiltering textureFiltering = TextureFiltering::Linear,
		TextureMipMapFiltering mipMapFiltering = TextureMipMapFiltering::LinearMipMapLinear, TextureWrapMode wrapMode = TextureWrapMode::Wrap, GLenum internalFormat = GL_RGBA);
	Texture2D* LoadTexture(const char* name);
	Texture2D* GetDefaultTexture() const { return defaultTexture; }
	const std::string& GetTexturePath() const { return path; }
	void Unload();
	
	TextureManager() = default;
	~TextureManager();
	TextureManager(const TextureManager& other) = delete;
	TextureManager& operator=(const TextureManager& other) = delete;

	//auto texture settings
	TextureFiltering textureFiltering;
	TextureMipMapFiltering mipMapFiltering;
	TextureWrapMode wrapMode;
	GLenum internalFormat;
private:
	std::string path;
	std::unordered_map<std::string, Texture2D*> map;
	Texture2D* defaultTexture;
	bool ownsDefault = false;
};

