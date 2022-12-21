#include "application.h"


Application::Application(int width, int height) :
	m_screenWidth{ width },
	m_screenHeight{ height },
	m_scene{ new Scene{} },
	m_camera{ new Camera{45.0f, width, height, glm::vec3{0.0f, 0.0f, 2.0f}} }
{
	m_scene->addSphere(glm::vec3{ 0, 0, -20 }, 5, RGB(255, 0, 0));
	m_scene->addSphere(glm::vec3{ 10, 10, -40 }, 5, RGB(0, 255, 0));

	m_scene->addPlane(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::vec3(0.0f, -4.0f, 0.0f), RGB(255, 255, 255));
}


void Application::captureInput(MSG* mptr)
{

	if (mptr->message == WM_LBUTTONDOWN) {
		m_pressedInputs[mptr->wParam] = true;
		m_mousePos.x = GET_X_LPARAM(mptr->lParam);
		m_mousePos.y = GET_Y_LPARAM(mptr->lParam);
	}
	else if (mptr->message == WM_LBUTTONUP) {
		m_pressedInputs[VK_LBUTTON] = false;
	}

	if (mptr->message == WM_MOUSEMOVE) {
		if (m_pressedInputs[VK_LBUTTON]) {
			glm::vec2 newMosPos = glm::vec2(GET_X_LPARAM(mptr->lParam), GET_Y_LPARAM(mptr->lParam));
			m_mouseOffset = newMosPos - m_mousePos;
			m_mousePos = newMosPos;
		}
	}


	if (mptr->message == WM_MOUSELEAVE) {
		m_mouseOffset = glm::vec2(0.0f);
		m_pressedInputs[VK_LBUTTON] = false;
	}

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



void Application::m_moveCamera()
{
	glm::vec3 direction{ 0 };
	bool isMoving = false;
	for (const auto &key: m_camMoveInputs)
	{
		if (m_pressedInputs[key])
		{
			direction += m_cameraDirections[key];
			isMoving = true;
		}
	}
	glm::normalize(direction);

	glm::vec3 rotation = getRotation();
	//if (isMoving) {
		m_camera->addRelativeOffset(direction * 0.1f);
		m_camera->addWorldRotation(rotation);
		m_camera->updateMatrices();
	//}
}


glm::vec3 Application::getRotation() {
	glm::vec3 rotation{};

	for (const auto& key : m_camRotateInputs) {
		if (m_pressedInputs[key])
		{
			rotation += m_cameraRotations[key] * 0.1f;
		}
	}

	if (m_pressedInputs[VK_LBUTTON] && glm::length(m_mouseOffset) > 0) {
		rotation.y += m_mouseOffset.x / (m_screenWidth / 2.0f) * -15.0f;
		rotation.x += m_mouseOffset.y / (m_screenHeight / 2.0f) * -15.0f;
		m_mouseOffset = glm::vec2{ 0.0f };
	}
	std::cout << glm::to_string(rotation) << std::endl;
	return rotation;
}


void Application::run(const WindowRenderData& winData) {

	m_scene->render(winData, m_camera);

	m_moveCamera();
}
