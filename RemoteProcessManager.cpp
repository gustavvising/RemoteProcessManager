#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include "ProcessHandler.h"


int main()
{
	ProcessHandler processHandler;

    processHandler.updateProcessList();
	processHandler.printProcessList();

	return EXIT_SUCCESS;
}