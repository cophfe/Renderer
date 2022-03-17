#include "Shader.h"

void Shader::Init(const char* path)
{
	//create the shader and get it's id
	id = glCreateShader(GL_VERTEX_SHADER);
	//save the file name, in case we need to recompile in the future
	path = path;

	lastTime = std::filesystem::last_write_time(path);
	Compile(false);
}

void Shader::Compile(bool recompile) //based on simpleframework
{
	const char* shader = LoadFileAsString(path).c_str();

	glShaderSource(id, 1, &shader, nullptr);
	glCompileShader(id);

	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		loaded = false;
		GLchar log[512];
		glGetShaderInfoLog(id, 512, nullptr, log);

		auto msg = "Failed to compile shader " + std::string(path) + ":\n" + log;
		if (!recompile)
		{
			throw msg;
			return;
		}
		else
		{
			std::cout << "Failed to recompile shader " + std::string(path) + ":\n" + log;
		}
	}
	loaded = true;
}

bool Shader::ShouldRecompile()
{
	auto time = std::filesystem::last_write_time(path);
	bool rcmp = time != lastTime;
	lastTime = time;

	return rcmp;
}

void Shader::Recompile()
{
	Compile(true);
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
		throw msg;
		return "";
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


