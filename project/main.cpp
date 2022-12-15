// project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>

#include <thread>

#include <glm/glm.hpp>

#include <windows.h>
#include <windowsx.h>

#include "utils/FPSTimer.h"
#include "windows/Window.h"
#include "source/math/ray.h"
#include "source/math/geometry/sphere.h"
#include "render/Scene.h"

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
	
	// Create window
	std::unique_ptr<Window<960, 540>> window = std::make_unique<Window<960, 540>>();
	std::unique_ptr<FPSTimer> timer = std::make_unique<FPSTimer>(60);
	std::unique_ptr<Scene> scene = std::make_unique<Scene>(window->getWidth(), window->getHeight());
	std::unique_ptr<Application> application = std::make_unique<Application>();

	bool running = true;

	// Event messages
	MSG msg = {0};
	while (running) {
		while (const auto peekRes = PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
			}
			// Translate keystroke message into correct format
			TranslateMessage(&msg);
			application->captureInput(&msg);
			// Send the message to WindowProc
			DispatchMessageW(&msg);

		}

		if (timer->frameElapsed()) {
			window->allocateBitmapBuffer();

			application->run(scene->getSphere());

			scene->render(window->getWindowData());

			window->flush();
			//scene->render(window);
			//std::cout << "ddd" << std::endl;
			// Here is the render code run at 60 FPS
			// printToVisualStudioOutput("Die");
		}
		std::this_thread::yield();
	}

	return msg.wParam;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
