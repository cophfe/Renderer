#pragma once
#include "Math.h"
#include "Graphics.h"
#include "Shader.h"
#include "Texture.h"

//this is a unity-esque material class
//Material will basically be wrappers for shader programs
//that means each material will have it's own uniform values
//these can be set using getfloat, setfloat etc

//ther is an exception: uniforms shared between all or most materials (like camera matrix and time and stuff) are stored in two uniform buffer objects (one for changing every frame, one for changing less than that)
//this is because uniform buffers can be shared accross all shader programs apparently, which sounds real useful

class Material
{
public:
	Material() { loaded = false; }

	Material* Init(Shader& vertex, Shader& fragment);
	static Material* InitNew(Shader& vertex, Shader& fragment);
	void Use() const;
	static void ClearMaterial();

	GLuint GetUniformID(const char* name) const;

#pragma region Uniform Set Get
	int GetInt(const char* name) const;
	float GetFloat(const char* name) const;
	Vector2&& GetVector2(const char* name) const;
	Vector3&& GetVector3(const char* name) const;
	Vector4&& GetVector4(const char* name) const;
	Matrix3x3&& GetMatrix3(const char* name) const;
	Matrix4x4&& GetMatrix4(const char* name) const;
	int GetInt(int id) const;
	float GetFloat(int id) const;
	Vector2&& GetVector2(int id) const;
	Vector3&& GetVector3(int id) const;
	Vector4&& GetVector4(int id) const;
	Matrix3x3&& GetMatrix3(int id) const;
	Matrix4x4&& GetMatrix4(int id) const;

	void SetUniform(const char* name, int value) const;
	void SetUniform(const char* name, float value) const;
	void SetUniform(const char* name, const Vector2& value) const;
	void SetUniform(const char* name, const Vector3& value) const;
	void SetUniform(const char* name, const Vector4& value) const;
	void SetUniform(const char* name, const Matrix3x3& value, GLboolean transpose = GL_FALSE) const;
	void SetUniform(const char* name, const Matrix4x4& value, GLboolean transpose = GL_FALSE) const;
	void SetUniform(int id, int value) const;
	void SetUniform(int id, float value) const;
	void SetUniform(int id, const Vector2& value) const;
	void SetUniform(int id, const Vector3& value) const;
	void SetUniform(int id, const Vector4& value) const;
	void SetUniform(int id, const Matrix3x3& value, GLboolean transpose = GL_FALSE) const;
	void SetUniform(int id, const Matrix4x4& value, GLboolean transpose = GL_FALSE) const;
#pragma endregion
	void SetTextureSampler(const char* name, Texture2D* texture);
	void SetTextureSampler(GLuint id, Texture2D* texture);

	inline bool IsLoaded() { return loaded; }
	inline GLuint GetProgramID() { return programID; }
	inline operator GLuint() { return programID; }

	inline bool UsesShader(Shader* shader) { return shader == fragment || shader == vertex; }
	void ReloadMaterial();

	Material* Clone();

	Material(Material&& other) noexcept;
	Material& operator= (Material&& other) noexcept;
	~Material();
	Material(const Material& other) = delete;
	Material& operator= (const Material& other) = delete;
private:
	static uint32_t materialIDCounter;
	static GLuint currentMaterial;
	uint32_t materialID;

	GLuint programID;
	bool loaded;

	struct TextureData
	{
		Texture2D* texture;
		GLuint attachedUnit;
		GLuint samplerID;
	};
	std::vector<TextureData> textures;

	Shader* vertex;
	Shader* fragment;
	//should also have geometry shader (i think)

	//needs to store an array of structs containing texture id and sampler id (i think)
};

