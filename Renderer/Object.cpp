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

void Object::UpdateUniform()
{
	transform.UpdateMatrix();

	Camera* cam = Program::GetInstance()->GetRenderer().GetMainCamera();
	material->SetUniform("MVPMatrix", cam->GetViewProjectionMatrix() * transform.GetMatrix());
}
