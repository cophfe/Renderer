#pragma once
#include "Graphics.h"

//this is a unity-esque material class
//Material will basically be wrappers for shader programs
//that means each material will have it's own uniform values
//these can be set using getfloat, setfloat etc

//ther is an exception: uniforms shared between all or most materials (like camera matrix and time and stuff) are stored in two uniform buffer objects (one for changing every frame, one for changing less than that)
//this is because uniform buffers can be shared accross all shader programs apparently, which sounds real useful

//anybody know why textures aren't set in the same way as uniforms in opengl? unless I'm mistaken most use cases require different textures per shader program, so it would be so much easier if you could just call SetTexture() or something on the shader program once and leave it at that, instead of binding it to a sampler every time it's rendered
class Material
{
public:
	void Init(Shader& vertex, Shader& fragment);
	void Use() const;

	GLuint GetUniformID(const char* name) const;

	int GetInt(const char* name) const;
	float GetFloat(const char* name) const;
	
	void SetUniform(const char* name, float value) const;
	void SetUniform(const char* name, float value) const;

	inline bool IsLoaded() { return loaded; }
	inline GLuint GetProgramID() { return id; }
	inline operator GLuint() { return id; }

	Material(Material&& other) noexcept;
	Material& operator= (Material&& other) noexcept;
	~Material();
	Material() = default;
	Material(const Material& other) = delete;
	Material& operator= (const Material& other) = delete;
private:
	
	GLuint id;
	bool loaded;

	Shader* vertex;
	Shader* fragment;
};

