#include "SubMesh.h"

void SubMesh::Render() const
{
	glBindVertexArray(vertexArray);

	if (indicesCount)
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
	else
		glDrawArrays(GL_TRIANGLES, 0, verticesCount);
}
