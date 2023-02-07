#pragma once

#include <iostream>

#include <windows.h>
#include <windowsx.h>

#include "Engine.hpp"

#include "utils/Timer/FPSTimer.h"
#include "windows/Window.h"


class Application
{
	static constexpr int WIN_WIDTH_DEF = 960;
	static constexpr int WIN_HEIGHT_DEF = 540;
	static constexpr int BUFF_DECREASE_TIMES = 2;
	inline static float BG_COLOR[] = { 0.2f, 0.2f, 0.2f, 1.0f };
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
	int m_screenWidth;
	int m_screenHeight;

	bool m_isRunning;

	// Application building blocks
	std::unique_ptr<engn::util::FPSTimer> m_timer;
	std::unique_ptr<engn::win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF, BUFF_DECREASE_TIMES>> m_window;
	std::unique_ptr<engn::Engine> m_engine;
};