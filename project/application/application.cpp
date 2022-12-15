#include "application.h"


Application::Application(): m_scene{ new Scene{} } {
	m_scene->addSphere(glm::vec3{ 400, 300, 20 }, 75, RGB(255, 0, 0));
	m_scene->addSphere(glm::vec3{ 100, 100, 20 }, 50, RGB(0, 255, 0));

	m_scene->addPlane(glm::normalize(glm::vec3(0.0f, 0.5f, 0.5f)), m_scene->getSphere().m_center + m_scene->getSphere().m_radius, RGB(255, 255, 255));
}


void Application::m_moveSphere(sphere &s)
{
	glm::vec3 direction{ 0 };
	for (const auto& [key, pressed] : m_pressedInputs)
	{
		if (pressed)
		{
			direction += m_directions[key];
		}
	}
	glm::normalize(direction);

	s.m_center += direction * 2.0f;
}


void Application::captureInput(MSG* mptr)
{
	// RMB down
	if (mptr->message == WM_RBUTTONDOWN)
	{
		m_pressedInputs[WM_RBUTTONDOWN] = true;
	}
	// RMB UP - Clear all inputs
	if (mptr->message == WM_RBUTTONUP)
	{
		for (auto& el : m_pressedInputs)
		{
			el.second = false;
		}
	}

	if (mptr->message == WM_KEYDOWN)
	{
		switch (mptr->wParam)
		{
		case WM_KEY_A: m_pressedInputs[WM_KEY_A] = true; break;
		case WM_KEY_D: m_pressedInputs[WM_KEY_D] = true; break;
		case WM_KEY_W: m_pressedInputs[WM_KEY_W] = true; break;
		case WM_KEY_S: m_pressedInputs[WM_KEY_S] = true; break;
		}
	}
	else if (mptr->message == WM_KEYUP)
	{
		switch (mptr->wParam)
		{
		case WM_KEY_A: m_pressedInputs[WM_KEY_A] = false; break;
		case WM_KEY_D: m_pressedInputs[WM_KEY_D] = false; break;
		case WM_KEY_W: m_pressedInputs[WM_KEY_W] = false; break;
		case WM_KEY_S: m_pressedInputs[WM_KEY_S] = false; break;
		}
	}
}


void Application::run(const WindowRenderData& winData) {

	m_scene->render(winData);

	m_moveSphere(m_scene->getSphere());
}
