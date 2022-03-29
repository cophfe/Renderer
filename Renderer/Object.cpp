#include "Object.h"
#include "Program.h"

void Object::Render()
{
	if (material && mesh)
	{
		material->Use();
		UpdateUniform();
		mesh->Render();
	}
}

void Object::UpdateUniformIDs()
{
	MVPMatrix = material->GetUniformID("_MVPMatrix");
	ModelMatrix = material->GetUniformID("_ModelMatrix");
}

void Object::UpdateUniform()
{
	Camera* cam = Program::GetInstance()->GetRenderer().GetMainCamera();

	material->SetUniform(MVPMatrix, cam->GetViewProjectionMatrix() * transform.GetLocalMatrix());
	material->SetUniform(ModelMatrix, transform.GetLocalMatrix());
}
