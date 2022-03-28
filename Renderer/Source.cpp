#include <iostream>
#include "Graphics.h"
#include "Program.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//try {
		Program::CreateInstance();
		Program::GetInstance()->Run();
		Program::DestroyInstance();
	//} 
	//catch (std::exception e)
	//{
	//	std::cout << e.what() << std::endl;
	//	std::cout << "Exiting program\n";
	//}
}