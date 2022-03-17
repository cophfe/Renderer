#pragma once
#include "Graphics.h"

class Texture
{
public:
	void Create();

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

private:
	GLuint id;
	bool created;
};

