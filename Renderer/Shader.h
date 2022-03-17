#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "Graphics.h"

class Shader
{
public:
	void Init(const char* path);

	bool ShouldRecompile();
	void Recompile();
	inline GLuint GetId() const { return id; }
	inline const std::string& GetFilePath() const { return path; }
	inline bool IsLoaded() const { return loaded; }

	static std::string LoadFileAsString(const std::string path);

	operator GLuint() const { return id; }

	Shader(Shader&& other) noexcept;
	Shader& operator= (Shader&& other) noexcept;
	~Shader();
	Shader() = default;
	Shader(const Shader& other) = delete;
	Shader& operator= (const Shader& other) = delete;

private:
	void Compile(bool recompile);

	std::filesystem::file_time_type lastTime;

	GLuint id;
	std::string path;
	bool loaded;

};

