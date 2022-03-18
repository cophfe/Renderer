#include "Material.h"

uint32_t Material::materialIDCounter = 0;

void Material::Init(Shader& vertex, Shader& fragment)
{
	this->vertex = &vertex;
	this->fragment = &fragment;

	programID = glCreateProgram();
	glAttachShader(programID, vertex);
	glAttachShader(programID, fragment);

	glLinkProgram(programID);

	GLint success = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		loaded = false;

		GLchar log[512];
		glGetShaderInfoLog(programID, 512, nullptr, log);

		auto msg = "Failed to link shader program using vertex: " + std::string(vertex.GetFilePath()) + "and fragment: " + std::string(fragment.GetFilePath()) + ":\n" + log;
		throw msg;
		return;
	}

	loaded = true;

	materialID = materialIDCounter;
	materialIDCounter++;
}

void Material::Use() const
{
	glUseProgram(programID);
}

void Material::ClearMaterial()
{
	glUseProgram(0);
	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Material::GetUniformID(const char* name) const
{
	return glGetUniformLocation(programID, name);
}

int Material::GetInt(const char* name) const
{
	return 0;
}

float Material::GetFloat(const char* name) const
{
	return 0;
}

void Material::SetUniform(const char* name, int value) const
{
}

void Material::SetUniform(const char* name, float value) const
{
}

Material::Material(Material&& other) noexcept
{
	fragment = other.fragment;
	vertex = other.vertex;
	programID = other.programID;
	loaded = other.loaded;

	other.loaded = false;
}

Material& Material::operator=(Material&& other) noexcept
{
	if (loaded)
	{
		glDeleteProgram(programID);
	}

	fragment = other.fragment;
	vertex = other.vertex;
	programID = other.programID;
	loaded = other.loaded;

	other.loaded = false;
	return *this;
}

Material::~Material()
{
	if (loaded)
	{
		glDeleteProgram(programID);
		loaded = false;
	}
}