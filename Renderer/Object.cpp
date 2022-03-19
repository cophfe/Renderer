#include "Object.h"

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
	material->SetUniform("objectMatrix", transform.GetMatrix());
}
