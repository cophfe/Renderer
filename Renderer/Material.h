#pragma once
#include "Graphics.h"
#include "Shader.h"

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

	void Init(Shader& vertex, Shader& fragment);
	void Use() const;
	static void ClearMaterial();

	GLuint GetUniformID(const char* name) const;

	int GetInt(const char* name) const;
	float GetFloat(const char* name) const;
	
	void SetUniform(const char* name, int value) const;
	void SetUniform(const char* name, float value) const;

	inline bool IsLoaded() { return loaded; }
	inline GLuint GetProgramID() { return programID; }
	inline operator GLuint() { return programID; }

	Material(Material&& other) noexcept;
	Material& operator= (Material&& other) noexcept;
	~Material();
	Material(const Material& other) = delete;
	Material& operator= (const Material& other) = delete;
private:
	static uint32_t materialIDCounter;
	uint32_t materialID;

	GLuint programID;
	bool loaded;

	Shader* vertex;
	Shader* fragment;
	//should also have geometry shader (i think)

	//needs to store an array of structs containing texture id and sampler id (i think)
};

