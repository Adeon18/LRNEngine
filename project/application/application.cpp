#include "application.h"


Application::Application(int width, int height) :
	m_scene{ new Scene{} },
	m_camera{ new Camera{45.0f, width, height, glm::vec3{0.0f, 0.0f, -2.0f}} }
{
	m_scene->addSphere(glm::vec3{ 0, 0, 20 }, 5, RGB(255, 0, 0));
	m_scene->addSphere(glm::vec3{ 10, 10, 40 }, 5, RGB(0, 255, 0));

	m_scene->addPlane(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::vec3(0.0f, -4.0f, 0.0f), RGB(255, 255, 255));
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

	s.m_center += direction * 0.1f;
}


void Application::captureInput(MSG* mptr)
{

	// Capture press and release of keys
	if (mptr->message == WM_KEYDOWN)
	{
		m_pressedInputs[mptr->wParam] = true;
		//std::cout << "Key Pressed: " << mptr->wParam << std::endl;
	}
	else if (mptr->message == WM_KEYUP)
	{
		m_pressedInputs[mptr->wParam] = false;
		//std::cout << "Key Released: " << mptr->wParam << std::endl;
	}
}


void Application::run(const WindowRenderData& winData) {

	m_scene->render(winData, m_camera);

	m_moveSphere(m_scene->getSphere());
}
