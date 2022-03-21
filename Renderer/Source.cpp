#include <iostream>
#include "Graphics.h"
#include "Program.h"

int main()
{
	Program::CreateInstance();
	Program::GetInstance()->Run();
	Program::DestroyInstance();
}