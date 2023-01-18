#include <thread>

#include "application.h"


#define RAYDRACER 0

Application::Application() :
	m_isRunning{ true },
	m_screenWidth{ WIN_WIDTH_DEF },
	m_screenHeight{ WIN_HEIGHT_DEF },
	m_engine{ new engn::Engine{} },
	m_scene{ new engn::Scene{} },
	m_timer{ new engn::util::FPSTimer{300} },
	m_window{ new engn::win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF, BUFF_DECREASE_TIMES>() },
	m_camera{ new engn::Camera{45.0f, WIN_WIDTH_DEF, WIN_HEIGHT_DEF, glm::vec3{0.0f, 0.0f, 2.0f}},
	}
{
#if RAYDRACER
	// Initialize the executor with half the threads if max_threads < 4 else with MAX_THREADS - 2
	uint32_t numThreads = (std::max)(1u, (std::max)(engn::util::ParallelExecutor::MAX_THREADS > 4u ? engn::util::ParallelExecutor::MAX_THREADS - 2u : 1u, engn::util::ParallelExecutor::HALF_THREADS));
	m_executor = std::unique_ptr<engn::util::ParallelExecutor>(new engn::util::ParallelExecutor{ numThreads });
	m_createObjects();
#endif
}


int Application::run() {

	MSG msg = { 0 };
	while (m_isRunning) {
		m_processWIN32Queue(&msg);

		if (m_timer->frameElapsed()) {
#if RAYDRACER
			if (m_window->allocateBitmapBuffer()) {
				setWindowSize(m_window->getWidth(), m_window->getHeight());
			}
			// Debug
			auto debug = m_timer->isDebugFPSShow();
			if (debug.first) { std::cout << "FPS: " << debug.second << std::endl; }

			m_handleRender();
			m_handlePhysics();

			m_window->flush();
#else
			float bgColor[] = {0.0f, 0.0f, 1.0f, 1.0f};
			m_window->setRenderTargetView();
			m_window->clear(bgColor);
			m_engine->render();
			m_window->present();
#endif
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
	m_scene->render(m_window->getWindowData(), m_camera, m_executor);
}


void Application::m_handlePhysics() {
	m_moveCamera();
	m_handleDragging();
}


void Application::m_createObjects() {
	m_scene->setDirectionalLight(
		glm::vec3{ 0.2f, -0.5f, -0.3f },
		engn::light::LightProperties{ glm::vec3{0.1f},  glm::vec3{0.2f}, glm::vec3{0.5f} }
	);

	m_scene->addPointLight(
		glm::vec3{ -4.0f, 2.0f, -7.0f },
		engn::light::LightProperties{ glm::vec3{0.1f},  glm::vec3{0.8f}, glm::vec3{1.0f} },
		engn::light::PLIGHT_DIST_50
	);

	m_scene->addPointLight(
		glm::vec3{ 4.0f, 2.0f, -7.0f },
		engn::light::LightProperties{ glm::vec3{0.1f},  glm::vec3{0.8f}, glm::vec3{1.0f} },
		engn::light::PLIGHT_DIST_50
	);

	m_scene->addSpotLight(
		glm::vec3{ 0.0f, -1.0f, 0.0f },
		glm::vec3{ 0.f, 4.0f, -20.0f },
		glm::vec2{ 0.9f, 0.7f },
		engn::light::LightProperties{ glm::vec3{0.05f},  glm::vec3{0.8f}, glm::vec3{1.0f} },
		engn::light::LIGHTPURPLE
	);

	// Emerald sphere
	m_scene->addRenderObject(
		new engn::math::sphere{ glm::vec3{ -3, 0, -20 }, 3 },
		engn::mtrl::EMERALD
	);

	m_scene->addRenderObject(
		new engn::math::sphere{ glm::vec3{ 3, 1, -20 }, 2 },
		engn::mtrl::RED_PLASTIC
	);

	// Whitw plastic plane
	m_scene->addRenderObject(
		new engn::math::plane{ glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.0f, -4.0f, 0.0f) },
		engn::mtrl::WHITE_PLASTIC
	);

	m_scene->addMesh("unit_box", engn::mesh::GET_BOX_MESH(glm::vec3{ -1, -1, -1 }, glm::vec3{ 1, 1, 1 }));

	m_scene->addRenderObject(
		m_scene->getMeshPtr("unit_box"),
		engn::mtrl::YELLOW_RUBBER,
		glm::vec3{ 2, -3, -10 }
	);

	m_scene->addRenderObject(
		m_scene->getMeshPtr("unit_box"),
		engn::mtrl::CHROME,
		glm::vec3{ -2, -3, -10 }
	);
}


void Application::setWindowSize(int width, int height) {
	m_screenWidth = width;
	m_screenHeight = height;

	m_camera->setNewScreenSize(width, height);
}


void Application::m_captureInput(MSG* mptr)
{

	// Capture and release LMB
	if (mptr->message == WM_LBUTTONDOWN) {
		m_onMouseLMBPressed(mptr);
		// std::cout << "LMB Down" << std::endl;
		// std::cout << "LMB Logged: " << m_pressedInputs[Keys::LMB] << std::endl;
	}
	else if (mptr->message == WM_LBUTTONUP) {
		m_onMouseLMBReleased(mptr);
		// std::cout << "LMB Up" << std::endl;
	}
	if (mptr->message == WM_RBUTTONDOWN) {
		m_onMouseRMBPressed(mptr);
		// std::cout << "RMB Down" << std::endl;
		// std::cout << "RMB Logged: " << m_pressedInputs[Keys::RMB] << std::endl;
	}
	else if (mptr->message == WM_RBUTTONUP) {
		m_onMouseRMBReleased(mptr);
		// std::cout << "RMB Up" << std::endl;
	}

	if (mptr->message == WM_MOUSEMOVE) {
		m_onMouseMove(mptr);
	}

	// Capture press and release of keys
	if (mptr->message == WM_KEYDOWN)
	{
		m_pressedInputs[mptr->wParam] = true;
	}
	else if (mptr->message == WM_KEYUP)
	{
		m_pressedInputs[mptr->wParam] = false;
	}
}


void Application::m_onMouseLMBPressed(MSG* mptr) {
	m_pressedInputs[Keys::LMB] = true;
	m_camMoveData.mousePos.x = GET_X_LPARAM(mptr->lParam);
	m_camMoveData.mousePos.y = GET_Y_LPARAM(mptr->lParam);
}


void Application::m_onMouseLMBReleased(MSG* mptr) {
	m_pressedInputs[Keys::LMB] = false;
}

void Application::m_onMouseRMBPressed(MSG* mptr) {
	m_pressedInputs[Keys::RMB] = true;
	m_objDragData.mousePos.x = GET_X_LPARAM(mptr->lParam);
	m_objDragData.mousePos.y = GET_Y_LPARAM(mptr->lParam);
}

glm::vec2 Application::m_processRMBInputs(const glm::vec2& mousePos) {
	glm::vec2 newPos;
	newPos = mousePos;
	newPos.y = m_window->getHeight() - mousePos.y;
	newPos /= static_cast<float>(BUFF_DECREASE_TIMES);
	return newPos;
}

void Application::m_onMouseRMBReleased(MSG* mptr) {
	m_pressedInputs[Keys::RMB] = false;
}

void Application::m_onMouseMove(MSG* mptr) {
	glm::vec2 newMosPos = glm::vec2(GET_X_LPARAM(mptr->lParam), GET_Y_LPARAM(mptr->lParam));
	if (m_pressedInputs[Keys::LMB]) {
		m_camMoveData.mouseOffset = newMosPos - m_camMoveData.mousePos;
		m_camMoveData.mousePos = newMosPos;
		// std::cout << "LMB Move" << std::endl;
	}
	if (m_pressedInputs[Keys::RMB]) {
		m_objDragData.mouseOffset = newMosPos - m_objDragData.mousePos;
		m_objDragData.mousePos = newMosPos;
		// std::cout << "RMB Move" << std::endl;
	}
}

void Application::m_findObject() {
	if (!m_objectBinded && (m_pressedInputs[Keys::RMB])) {
		if (m_scene->findDraggable(m_processRMBInputs(m_objDragData.mousePos), m_camera)) {
			m_objectBinded = true;
		}
	}
}

void Application::m_moveObject() {
	if (m_objectBinded && (m_pressedInputs[Keys::RMB])) {
		// second operation expensive
		if (m_camStateChanged || glm::length(m_objDragData.mouseOffset) > 0) {
			m_scene->moveDraggable(m_processRMBInputs(m_objDragData.mousePos), m_camera);
			m_camStateChanged = false;
		}
	}
}

void Application::m_releaseObject() {
	if (!m_pressedInputs[Keys::RMB]) {
		m_objectBinded = false;
	}
}

void Application::m_handleDragging() {
	m_findObject();
	m_moveObject();
	m_releaseObject();
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
		m_isCamMoving = false;
		m_isCamRotating = false;
		m_camStateChanged = true;
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
	if (m_pressedInputs[Keys::LMB] && glm::length(m_camMoveData.mouseOffset) > 0) {
		if (!m_isCamRotating) { m_isCamRotating = true; }
		float lastFPSCount = m_timer->getFPSCurrent();
		// hardcoded for now
		rotation.y += m_camMoveData.mouseOffset.x / (m_screenWidth / 2.0f) * -180.0f / 10.0f;
		rotation.x += m_camMoveData.mouseOffset.y / (m_screenHeight / 2.0f) * -180.0f / 10.0f;
		m_camMoveData.mouseOffset = glm::vec2{ 0.0f };
	}

	return rotation;
}
