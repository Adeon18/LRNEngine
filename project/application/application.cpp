#include <thread>

#include "application.h"


Application::Application() :
	m_isRunning{ true },
	m_screenWidth{ WIN_WIDTH_DEF },
	m_screenHeight{ WIN_HEIGHT_DEF },
	m_scene{ new Scene{} },
	m_timer{ new FPSTimer{300} },
	m_window{ new Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF>() },
	m_camera{ new Camera{45.0f, WIN_WIDTH_DEF, WIN_HEIGHT_DEF, glm::vec3{0.0f, 0.0f, 2.0f}} }
{
	m_createObjects();
}


int Application::run() {

	MSG msg = { 0 };
	while (m_isRunning) {
		m_processWIN32Queue(&msg);

		if (m_timer->frameElapsed()) {
			if (m_window->allocateBitmapBuffer()) {
				setWindowSize(m_window->getWidth(), m_window->getHeight());
			}

			m_handleRender();
			m_handlePhysics();

			m_window->flush();
		}
		std::this_thread::yield();
	}

	return msg.wParam;
}


void Application::m_processWIN32Queue(MSG* mptr) {
	while (const auto peekRes = PeekMessageW(mptr, nullptr, 0, 0, PM_REMOVE)) {
		if (mptr->message == WM_QUIT) {
			m_isRunning = false;
		}
		// Translate keystroke message into correct format
		TranslateMessage(mptr);
		// Capture input into application
		this->m_captureInput(mptr);
		// Send the message to WindowProc
		DispatchMessageW(mptr);
	}
}


void Application::m_handleRender() {
	m_scene->render(m_window->getWindowData(), m_camera);
}


void Application::m_handlePhysics() {
	m_moveCamera();
}


void Application::m_createObjects() {
	m_scene->setDirectionalLight(glm::vec3{ 0.2f, -0.5f, -0.3f }, light::LightProperties{ glm::vec3{0.1f},  glm::vec3{0.4f}, glm::vec3{0.2f}, });
	
	m_scene->addPointLight(glm::vec3{ 3.0f, -3.0f, -25.0f }, light::LightProperties{ glm::vec3{0.1f},  glm::vec3{0.8f}, glm::vec3{1.0f} }, glm::vec3{ 1.0f, 0.09f, 0.032f });
	//m_scene->addPointLight(glm::vec3{ -6.0f, 6.0f, -15.0f }, light::LightProperties{ glm::vec3{0.1f},  glm::vec3{0.8f}, glm::vec3{1.0f} }, glm::vec3{ 1.0f, 0.07f, 0.017f });
	
	m_scene->addSphereObject(
		new sphere{ glm::vec3{ 0, 0, -20 }, 5},
		Material{ glm::vec3{0.0215f, 0.1745f, 0.0215f}, glm::vec3{0.07568f, 0.61424f, 0.07568f}, glm::vec3{0.633f, 0.727811f, 0.633f}, 0.6f * 128 });
	//m_scene->addSphereObject(glm::vec3{ 10, 10, -40 }, 5, RGB(0, 255, 0));
	m_scene->addPlaneObject(
		new plane{ glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::vec3(0.0f, -4.0f, 0.0f) },
		Material{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.55f, 0.55f, 0.55f}, glm::vec3{0.7f, 0.7f, 0.7f}, 0.25f * 128 });
}


void Application::setWindowSize(int width, int height) {
	m_screenWidth = width;
	m_screenHeight = height;

	m_camera->setNewScreenSize(width, height);
}


void Application::m_captureInput(MSG* mptr)
{

	if (mptr->message == WM_LBUTTONDOWN) {
		m_onMouseLMBPressed(mptr);
	}
	else if (mptr->message == WM_LBUTTONUP) {
		m_onMouseLMBReleased(mptr);
	}

	if (mptr->message == WM_MOUSEMOVE) {
		m_onMouseMove(mptr);
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


void Application::m_onMouseLMBPressed(MSG* mptr) {
	m_pressedInputs[mptr->wParam] = true;
	m_mousePos.x = GET_X_LPARAM(mptr->lParam);
	m_mousePos.y = GET_Y_LPARAM(mptr->lParam);
}


void Application::m_onMouseLMBReleased(MSG* mptr) {
	m_pressedInputs[VK_LBUTTON] = false;
}


void Application::m_onMouseMove(MSG* mptr) {
	if (m_pressedInputs[VK_LBUTTON]) {
		glm::vec2 newMosPos = glm::vec2(GET_X_LPARAM(mptr->lParam), GET_Y_LPARAM(mptr->lParam));
		m_mouseOffset = newMosPos - m_mousePos;
		m_mousePos = newMosPos;
	}
}



void Application::m_moveCamera()
{
	glm::vec3 rotation = m_getCamRotation();
	glm::vec3 direction = m_getCamMovement();


	if (m_isCamMoving) {
		m_camera->addRelativeOffset(direction * 0.1f);
	}

	if (m_isCamRotating) {
		m_camera->addWorldRotation(rotation);
	}

	if (m_isCamMoving || m_isCamRotating) {
		m_camera->updateMatrices();
	}
}


glm::vec3 Application::m_getCamMovement() {
	glm::vec3 direction{ 0.0f };
	m_isCamMoving = false;
	for (const auto& key : m_camMoveInputs)
	{
		if (m_pressedInputs[key])
		{
			direction += m_cameraDirections[key];
			m_isCamMoving = true;
		}
	}
	glm::normalize(direction);

	return direction;
}


glm::vec3 Application::m_getCamRotation() {
	glm::vec3 rotation{};
	m_isCamRotating = false;
	// Roll
	for (const auto& key : m_camRotateInputs) {
		if (m_pressedInputs[key])
		{
			rotation += m_cameraRotations[key];
			if (!m_isCamRotating) { m_isCamRotating = true; }
		}
	}

	// pitch and yaw
	if (m_pressedInputs[VK_LBUTTON] && glm::length(m_mouseOffset) > 0) {
		if (!m_isCamRotating) { m_isCamRotating = true; }
		float lastFPSCount = m_timer->getFPSCurrent();
		// hardcoded for now
		rotation.y += m_mouseOffset.x / (m_screenWidth / 2.0f) * -180.0f / 10.0f;
		rotation.x += m_mouseOffset.y / (m_screenHeight / 2.0f) * -180.0f / 10.0f;
		m_mouseOffset = glm::vec2{ 0.0f };
	}

	return rotation;
}
