#pragma once

#include <iostream>

#include <windows.h>
#include <windowsx.h>

#include "Engine.hpp"

#include "utils/Timer/FPSTimer.h"
#include "windows/Window.h"


class Application
{
	//! FPS which the timer tries to tick at
	static constexpr float TIMER_FPS = 300.0f;
public:
	Application();
	//! Run and render the application
	int run();
private:
	//! Process the win32 API message queue
	void m_processWIN32Queue(MSG* mptr);
	//! A function responsible for scene render
	void m_handleRender();

	//! Capture the input into a map
	void m_captureInput(MSG* mptr);

private:

	bool m_isRunning;

	// Application building blocks
	std::unique_ptr<engn::util::FPSTimer> m_timer;
	std::unique_ptr<engn::Engine> m_engine;
};