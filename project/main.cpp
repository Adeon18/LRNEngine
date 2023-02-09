// project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


// Basically tell the linker to look for these libraries
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11.lib")

#include <iostream>
#include <sstream>

#include "render/D3D/d3d.hpp"

#include <windows.h>
#include <windowsx.h>

#include "Engine.hpp"
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

	// Initialize engine
	engn::Engine::init();

	// Create application
	std::unique_ptr<Application> application = std::make_unique<Application>();
	auto res = application->run();

	engn::Engine::deinit();
	return res;
}
