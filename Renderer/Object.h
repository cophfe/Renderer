#pragma once
#include "Graphics.h"
#include "Math.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

class Object 
{
public:
	Object(Mesh* mesh, Material* material) : mesh(mesh), material(material), transform(Transform()) { UpdateUniformIDs(); }

	void Render();
	inline Transform& GetTransform() { return transform; };
private:
	void UpdateUniformIDs();

	void UpdateUniform();

	Transform transform;
	Mesh* mesh;
	Material* material;

	GLuint MVPMatrix;
	GLuint ModelMatrix;
};

