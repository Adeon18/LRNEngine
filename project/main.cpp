// project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>

#include "render/D3D/d3d.hpp"

#include <windows.h>
#include <windowsx.h>

#include "application/application.h"


template<typename ...Args>
void printToVisualStudioOutput(Args&&... args)
{
	std::wstringstream ss;
	(ss << ... << args) << std::endl; // Fold expression requires C++17
	OutputDebugStringW(ss.str().c_str());
}


void initConsole()
{
	AllocConsole();
	FILE* dummy;
	auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	// Initialize Console
	initConsole();

	// Create application
	std::unique_ptr<Application> application = std::make_unique<Application>();

	return application->run();
}
