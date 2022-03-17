#pragma once
#include "Graphics.h"

class Program
{
public:
	void Run();

private:
	void Init();
	void Loop();
	void Cleanup();

	GLFWwindow* window;
};

