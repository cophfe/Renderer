#include "Shader.h"

Shader* Shader::Init(const char* path, Type type)
{
	if (loaded)
	{
		auto msg = "Failed to create shader using path: " + std::string(path) + ": Shader used was already loaded.";
		throw std::runtime_error(msg);
	}

	//create the shader and get it's id
	id = glCreateShader((GLenum)type);
	//save the file name, in case we need to recompile in the future
	this->path = path;

	lastTime = std::filesystem::last_write_time(path);
	Compile(false);
	loaded = true;
	return this;
}

Shader* Shader::InitNew(const char* path, Type type)
{
	Shader* shader = new Shader();
	shader->Init(path, type);
	return shader;
}

bool Shader::Compile(bool recompile) //based on simpleframework
{
	std::string shader = LoadFileAsString(path).c_str();
	const char* cStr = shader.c_str();

	glShaderSource(id, 1, &cStr, nullptr);
	glCompileShader(id);

	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar log[512];
		glGetShaderInfoLog(id, 512, nullptr, log);

		if (!(recompile && loaded))
		{
			loaded = false;
			auto msg = "Failed to compile shader " + std::string(path) + ":\n" + log;
			throw std::runtime_error(msg);
			return false;
		}
		else
		{
			std::cout << "Failed to recompile shader " + std::string(path) + ":\n" + log << std::endl;
			return false;
		}
	}

	return true;
}

bool Shader::ShouldRecompile()
{
	auto time = std::filesystem::last_write_time(path);
	bool rcmp = time != lastTime;
	lastTime = time;

	return rcmp;
}

bool Shader::Recompile()
{
	return Compile(true);
}

std::string Shader::LoadFileAsString(std::string path)
{
	auto file = std::ifstream(path);

	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close(); // don't really need to call this but whatever safety first

		return buffer.str();
	}
	else
	{
		auto msg = "Failed to load file " + std::string(path);
		throw std::runtime_error(msg);
	}

}

Shader::Shader(Shader&& other) noexcept
{
	loaded = other.loaded;
	id = other.id;
	path = other.path;

	other.loaded = false;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (loaded)
	{
		glDeleteShader(id);
	}

	loaded = other.loaded;
	id = other.id;
	path = other.path;
	
	other.loaded = false;
	return *this;
}

Shader::~Shader()
{
	if (loaded)
	{
		glDeleteShader(id);
		loaded = false;
	}
}


