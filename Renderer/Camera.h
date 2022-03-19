#pragma once
#include "Math.h"
#include "Graphics.h"
#include "Transform.h"

class Camera
{

private:
	Matrix4x4 projectionMatrix;
	Transform transform;
};

