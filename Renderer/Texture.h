#pragma once
#include "Graphics.h"

//to be implemented
class Texture
{
public:
	//static Texture&& Create(Vector2Int size, Format format, int mipMapDepth, Filtering filtering);
	//static Texture&& Create(Vector2Int size, Format format, int mipMapDepth, MipMapFiltering filtering);
	//static Texture&& CreateFromFile(const char* path);

	enum class Filtering
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};
	enum class MipMapFiltering
	{
		NearestMipMapNearest,
		NearestMipMapLinear,
		LinearMipMapNearest,
		LinearMipMapLinear
	};
	enum class Format
	{

	};

	//~Texture();
private:
	Texture() { created = false; }
	//Texture(Texture&& other) noexcept;
	//Texture& operator= (Texture&& other) noexcept;
	
	Texture(const Texture& other) = delete;
	Texture& operator= (const Texture& other) = delete;

	GLuint id;
	bool created;
};

