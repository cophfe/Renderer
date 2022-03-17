#include "Material.h"

void Material::Init(Shader& vertex, Shader& fragment)
{
	this->vertex = &vertex;
	this->fragment = &fragment;

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);

	glLinkProgram(id);

	GLint success = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		loaded = false;

		GLchar log[512];
		glGetShaderInfoLog(id, 512, nullptr, log);

		auto msg = "Failed to link shader program using vertex: " + std::string(vertex.GetFilePath()) + "and fragment: " + std::string(fragment.GetFilePath()) + ":\n" + log;
		throw msg;
		return;
	}

	loaded = true;
}

void Material::Use() const
{
	glUseProgram(id);
}

GLuint Material::GetUniformID(const char* name) const
{
	return glGetUniformLocation(id, name);
}

int Material::GetInt(const char* name) const
{
	
}

float Material::GetFloat(const char* name) const
{

}

Material::Material(Material&& other) noexcept
{
	fragment = other.fragment;
	vertex = other.vertex;
	id = other.id;
	loaded = other.loaded;

	other.loaded = false;
}

Material& Material::operator=(Material&& other) noexcept
{
	if (loaded)
	{
		glDeleteProgram(id);
	}

	fragment = other.fragment;
	vertex = other.vertex;
	id = other.id;
	loaded = other.loaded;

	other.loaded = false;
	return *this;
}

Material::~Material()
{
	if (loaded)
	{
		glDeleteProgram(id);
		loaded = false;
	}
}