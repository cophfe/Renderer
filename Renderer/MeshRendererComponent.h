#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

class MeshRendererComponent :
    public Component
{
public:
	void Init(Mesh* mesh, Material* material);
	void Render();
private:
	// Inherited via Component
	virtual Component* Clone() const override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;
	virtual void Unload() override;

	Mesh* mesh;
	Material* material;

	GLuint MVPMatrix;
	GLuint ModelMatrix;
};

