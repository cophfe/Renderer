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
	void Render() const;

	inline void SetMesh(Mesh* mesh) { this->mesh = mesh; }
	inline void SetMaterial(Material* material) { this->material = material; }
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

