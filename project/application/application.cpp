#include "input/Mouse.hpp"
#include "input/Keyboard.hpp"

#include "application.h"


Application::Application() :
	m_isRunning{ true },
	m_screenWidth{ WIN_WIDTH_DEF },
	m_screenHeight{ WIN_HEIGHT_DEF },
	m_timer{ new engn::util::FPSTimer{TIMER_FPS} },
	m_window{ new engn::win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF, BUFF_DECREASE_TIMES>() },
	m_engine{ new engn::Engine{WIN_WIDTH_DEF, WIN_HEIGHT_DEF} }
{}


int Application::run() {

	MSG msg = { 0 };
	while (m_isRunning) {
		m_processWIN32Queue(&msg);

		if (m_timer->frameElapsed()) {
			auto debug = m_timer->isDebugFPSShow();
			if (debug.first) { engn::Logger::instance().logDebug("FPS", debug.second); }

			m_handleRender();
			engn::inp::Keyboard::getInstance().fillPrevFrameKeys();
		}
		std::this_thread::yield();
	}

	return msg.wParam;
}


void Application::m_processWIN32Queue(MSG* mptr) {
	while (const auto peekRes = PeekMessageW(mptr, NULL, 0, 0, PM_REMOVE)) {
		// Translate keystroke message into correct format
		TranslateMessage(mptr);
		// Capture input into application
		this->m_captureInput(mptr);
		// Send the message to WindowProc
		DispatchMessageW(mptr);

		if (mptr->message == WM_QUIT) {
			m_isRunning = false;
		}
	}
}


void Application::m_handleRender() {
	// Get the render data for the shader
	engn::rend::RenderData renderData{
		m_timer->getSecondsSinceStart(),
		m_window->getWidth(),
		m_window->getHeight(),
		1.0f / m_window->getWidth(),
		1.0f / m_window->getHeight(),
	};
	// Render fucntions
	if (m_window->clear(BG_COLOR)) {
		m_engine->setWindowSize(m_window->getWidth(), m_window->getHeight());
	}
	m_engine->handlePhysics(renderData);
	m_engine->render(renderData);
	m_window->present();
}


void Application::m_captureInput(MSG* mptr)
{
	switch (mptr->message) {
	case WM_LBUTTONDOWN:
		engn::inp::Mouse::getInstance().onLMBPressed(mptr);
		break;
	case WM_LBUTTONUP:
		engn::inp::Mouse::getInstance().onLMBReleased(mptr);
		break;
	case WM_RBUTTONDOWN:
		engn::inp::Mouse::getInstance().onRMBPressed(mptr);
		break;
	case WM_RBUTTONUP:
		engn::inp::Mouse::getInstance().onRMBReleased(mptr);
		break;
	case WM_MOUSEMOVE:
		engn::inp::Mouse::getInstance().onMove(mptr);
		break;
	case WM_KEYDOWN:
		engn::inp::Keyboard::getInstance().onKeyPressed(mptr);
		break;
	case WM_KEYUP:
		engn::inp::Keyboard::getInstance().onKeyReleased(mptr);
		break;
	}

}
