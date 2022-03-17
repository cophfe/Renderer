#include "Program.h"

void Program::Run()
{
	Init();
	Loop();
	Cleanup();
}

void Program::Init()
{
	GLFWwindow* window;
	if (!glfwInit())
	{
		throw "GLFW failed to initiate.";
		return;
	}

	window = glfwCreateWindow(1280, 720, "yo", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		throw "GLFW failed to create window.";
		return;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		throw "GLAD failed to load.";
		return;
	}

	glEnable(GL_DEPTH_TEST);
}

void Program::Loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

void Program::Cleanup()
{
	glfwTerminate();
}
