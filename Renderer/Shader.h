#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <exception>

#include "Graphics.h"

class Shader
{
public:
	enum class Type : GLenum
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Compute = GL_COMPUTE_SHADER,
		Geometry = GL_GEOMETRY_SHADER
	};

	Shader() { loaded = false; }
	Shader* Init(const char* path, Type type);
	static Shader* InitNew(const char* path, Type type);

	bool ShouldRecompile();
	bool Recompile();
	inline GLuint GetId() const { return id; }
	inline const std::string& GetFilePath() const { return path; }
	inline bool IsLoaded() const { return loaded; }

	static std::string LoadFileAsString(const std::string path);

	operator GLuint() const { return id; }

	Shader(Shader&& other) noexcept;
	Shader& operator= (Shader&& other) noexcept;
	~Shader();
	Shader(const Shader& other) = delete;
	Shader& operator= (const Shader& other) = delete;

private:
	bool Compile(bool recompile);

	std::filesystem::file_time_type lastTime;

	GLuint id;
	std::string path;
	bool loaded;

};

