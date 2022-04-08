#include "MeshRendererComponent.h"
#include "Program.h"

void MeshRendererComponent::Init(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;

	if (material)
	{
		MVPMatrix = material->GetUniformID("_MVPMatrix");
		ModelMatrix = material->GetUniformID("_ModelMatrix");
	}
}

void MeshRendererComponent::Render() const
{
	if (material && mesh)
	{
		material->Use();
		//Update uniforms
		CameraComponent* cam = Program::GetInstance()->GetRenderer().GetMainCamera();

		const Transform& t = GetGameObject()->ReadTransform();
		material->SetUniform(MVPMatrix, cam->GetViewProjectionMatrix() * t.GetLocalMatrix());
		material->SetUniform(ModelMatrix, t.GetLocalMatrix());

		mesh->Render();
	}
}

Component* MeshRendererComponent::Clone() const
{
    return new MeshRendererComponent(*this);
}

void MeshRendererComponent::Start()
{
}

void MeshRendererComponent::Update()
{
}

void MeshRendererComponent::OnDisable()
{
	//unregister from renderer
	Program::GetInstance()->GetRenderer().DeregisterRenderer(this);
}

void MeshRendererComponent::OnEnable()
{
	//register to renderer
	Program::GetInstance()->GetRenderer().RegisterRenderer(this);
}

void MeshRendererComponent::Unload()
{
}
