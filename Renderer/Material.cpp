#include "Material.h"
#include "Program.h"

int32_t Material::materialIDCounter = 0;
GLuint Material::currentMaterial = -1;

Material* Material::Init(Shader& vertex, Shader& fragment)
{
	if (loaded)
	{
		auto msg = "Failed to create material using vertex: " + std::string(vertex.GetFilePath()) + "and fragment: " + std::string(fragment.GetFilePath()) + ":\nMaterial used was already loaded.";
		throw std::runtime_error(msg);
	}

	this->vertex = &vertex;
	this->fragment = &fragment;

	//this be based on finn's implimentation
	programID = glCreateProgram();
	glAttachShader(programID, vertex);
	glAttachShader(programID, fragment);
	glLinkProgram(programID);

	GLint success = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		loaded = false;

		GLchar log[512];
		glGetProgramInfoLog(programID, 512, NULL, log);
		auto msg = "Failed to link shader program using vertex: " + std::string(vertex.GetFilePath()) + "and fragment: " + std::string(fragment.GetFilePath()) + ":\n" + log;
		throw std::runtime_error(msg);
	}

	loaded = true;

	materialID = materialIDCounter;
	materialIDCounter++;

	return this;
}

Material* Material::InitNew(Shader& vertex, Shader& fragment)
{
	Material* material = new Material;

	auto& r = Program::GetInstance()->GetRenderer();
	if (r.GetAutoRegisterMaterials())
		r.RegisterMaterial(material);

	return material->Init(vertex, fragment);
}

void Material::Use() const
{
	if (currentMaterial != materialID)
	{
		glUseProgram(programID);
		currentMaterial = materialID;

		//Set textures
		for (auto& tex : textures)
		{
			glActiveTexture(GL_TEXTURE0 + tex.attachedUnit);
			glBindTexture(GL_TEXTURE_2D, tex.texture ? tex.texture->GetID() : 0);
		}
	}
}

void Material::ClearMaterial()
{
	glUseProgram(0);
	currentMaterial = -1;
}

GLuint Material::GetUniformID(const char* name) const
{
	return glGetUniformLocation(programID, name);
}

#pragma region Get Uniform
int Material::GetInt(const char* name) const
{
	return GetInt(glGetUniformLocation(programID, name));
}

float Material::GetFloat(const char* name) const
{
	return GetFloat (glGetUniformLocation(programID, name));
}

Vector2&& Material::GetVector2(const char* name) const
{
	return GetVector2(glGetUniformLocation(programID, name));
}

Vector3&& Material::GetVector3(const char* name) const
{
	return GetVector3(glGetUniformLocation(programID, name));
}

Vector4&& Material::GetVector4(const char* name) const
{
	return GetVector4(glGetUniformLocation(programID, name));
}

Matrix3x3&& Material::GetMatrix3(const char* name) const
{
	return GetMatrix3(glGetUniformLocation(programID, name));
}

Matrix4x4&& Material::GetMatrix4(const char* name) const
{
	return GetMatrix4(glGetUniformLocation(programID, name));
}

int Material::GetInt(int id) const
{
	int value;
	glGetUniformiv(programID, id, &value);
	return value;
}

float Material::GetFloat(int id) const
{
	float value;
	glGetUniformfv(programID, id, &value);
	return value;
}

Vector2&& Material::GetVector2(int id) const
{
	Vector2 value;
	glGetnUniformfv(programID, id, 2, (GLfloat*)&value[0]);
	return std::move(value);
}

Vector3&& Material::GetVector3(int id) const
{
	Vector3 value;
	glGetnUniformfv(programID, id, 3, (GLfloat*)&value[0]);
	return std::move(value);
}

Vector4&& Material::GetVector4(int id) const
{
	Vector4 value;
	glGetnUniformfv(programID, id, 4, (GLfloat*)&value[0]);
	return std::move(value);
}

Matrix3x3&& Material::GetMatrix3(int id) const
{
	Matrix3x3 value;
	glGetnUniformfv(programID, id, 9, (GLfloat*)&value[0]); //these values are probably organised incorrectly
	return std::move(value);
}

Matrix4x4&& Material::GetMatrix4(int id) const
{
	Matrix4x4 value;
	glGetnUniformfv(programID, id, 16, (GLfloat*)&value[0]);
	return std::move(value);
}
#pragma endregion

#pragma region Set Uniform
void Material::SetUniform(const char* name, int value) const
{
	SetUniform(glGetUniformLocation(programID, name), value);
}

void Material::SetUniform(const char* name, float value) const
{
	SetUniform(glGetUniformLocation(programID, name), value);
}

void Material::SetUniform(const char* name, const Vector2& value) const
{
	SetUniform(glGetUniformLocation(programID, name), value);
}

void Material::SetUniform(const char* name, const Vector3& value) const
{
	SetUniform(glGetUniformLocation(programID, name), value);
}

void Material::SetUniform(const char* name, const Vector4& value) const
{
	SetUniform(glGetUniformLocation(programID, name), value);
}

void Material::SetUniform(const char* name, const Matrix3x3& value, GLboolean transpose) const
{
	SetUniform(glGetUniformLocation(programID, name), value, transpose);
}

void Material::SetUniform(const char* name, const Matrix4x4& value, GLboolean transpose) const
{
	SetUniform(glGetUniformLocation(programID, name), value, transpose);
}

void Material::SetTextureSampler(const char* name, Texture2D* texture)
{
	SetTextureSampler(glGetUniformLocation(programID, name), texture);
}

void Material::SetUniform(int id, int value) const
{
	Use();
	glUniform1i(id, value);
}

void Material::SetUniform(int id, float value) const
{
	Use();
	glUniform1f(id, value);
}

void Material::SetUniform(int id, const Vector2& value) const
{
	Use();
	glUniform2f(id, value.x, value.y);
}

void Material::SetUniform(int id, const Vector3& value) const
{
	Use();
	glUniform3f(id, value.x, value.y, value.z);
}

void Material::SetUniform(int id, const Vector4& value) const
{
	Use();
	glUniform4f(id, value.x, value.y, value.z, value.w);
}

void Material::SetUniform(int id, const Matrix3x3& value, GLboolean transpose) const
{
	Use();
	glUniformMatrix3fv(id, 1, transpose, (GLfloat*)&(value[0]));
}

void Material::SetUniform(int id, const Matrix4x4& value, GLboolean transpose) const
{
	Use();
	glUniformMatrix4fv(id, 1, transpose, (GLfloat*)&(value[0]));
}

void Material::SetTextureSampler(GLuint id, Texture2D* texture)
{
	Use();

	//If this sampler has been set before, change texture to this texture but don't change the id
	for (size_t i = 0; i < textures.size(); i++)
	{
		if (textures[i].samplerID == id)
		{
			textures[i].texture = texture;
			//set textures (necessary for very sepecific bug)
			for (auto& tex : textures)
			{
				glActiveTexture(GL_TEXTURE0 + tex.attachedUnit);
				glBindTexture(GL_TEXTURE_2D, tex.texture ? tex.texture->GetID() : 0);
			}
			return;
		}
	}

	//Otherwise get the smallest unchosen textureunit and set this unit to it (this is recursive, kinda yikes)
	GLuint smallestUnchosenUnit = 0;
	for (size_t i = 0; i < textures.size(); i++)
	{
		if (textures[i].attachedUnit == smallestUnchosenUnit)
		{
			smallestUnchosenUnit++;
			i = 0;
		}
	}

	TextureData data;
	data.attachedUnit = smallestUnchosenUnit;
	data.texture = texture;
	data.samplerID = id;
	glUniform1i(id, data.attachedUnit);
	textures.push_back(data);
	//set textures (necessary for very sepecific bug)
	for (auto& tex : textures)
	{
		glActiveTexture(GL_TEXTURE0 + tex.attachedUnit);
		glBindTexture(GL_TEXTURE_2D, tex.texture ? tex.texture->GetID() : 0);
	}
}
#pragma endregion

void Material::ReloadMaterial()
{
	glAttachShader(programID, vertex->GetId());
	glAttachShader(programID, fragment->GetId());
	glLinkProgram(programID);

	GLint success = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		auto msg = "Failed to reload material using vertex: " + std::string(vertex->GetFilePath()) + " and fragment: " + std::string(fragment->GetFilePath()) + "\n";
		std::cout << msg;
	}
}

Material* Material::Clone()
{
	//note: does not contain the same uniform values
	Material* mat = new Material();
	mat->Init(*vertex, *fragment);

	auto& r = Program::GetInstance()->GetRenderer();
	if (r.GetAutoRegisterMaterials())
		r.RegisterMaterial(mat);
	return mat;
}

Material::Material(Material&& other) noexcept
{
	fragment = other.fragment;
	vertex = other.vertex;
	programID = other.programID;
	loaded = other.loaded;
	materialID = other.materialID;
	textures = other.textures;

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
	materialID = other.materialID;
	textures = other.textures;

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