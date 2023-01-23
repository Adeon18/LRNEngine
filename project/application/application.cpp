#include <thread>


#include "input/Mouse.hpp"
#include "input/Keyboard.hpp"

#include "application.h"



Application::Application() :
	m_isRunning{ true },
	m_screenWidth{ WIN_WIDTH_DEF },
	m_screenHeight{ WIN_HEIGHT_DEF },
	m_timer{ new engn::util::FPSTimer{300} },
	m_window{ new engn::win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF, BUFF_DECREASE_TIMES>() },
#if DX_ENGINE == 1
	m_engine{ new engn::Engine{} }
#else
	m_scene{ new engn::Scene{} },
	m_camera{ new engn::Camera{45.0f, WIN_WIDTH_DEF, WIN_HEIGHT_DEF, glm::vec3{0.0f, 0.0f, 2.0f}}}
#endif
{
#if DX_ENGINE == 0
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
#if DX_ENGINE == 1
			auto debug = m_timer->isDebugFPSShow();
			if (debug.first) { std::cout << "FPS: " << debug.second << std::endl; }

			m_handleRender();
#else
			if (m_window->allocateBitmapBuffer()) {
				setWindowSize(m_window->getWidth(), m_window->getHeight());
			}
			// Debug
			auto debug = m_timer->isDebugFPSShow();
			if (debug.first) { std::cout << "FPS: " << debug.second << std::endl; }

			m_handleRender();
			m_handlePhysics();

			m_window->flush();
#endif
		}
		std::this_thread::yield();
	}

	return msg.wParam;
}


void Application::setWindowSize(int width, int height) {
	m_screenWidth = width;
	m_screenHeight = height;

	m_camera->setNewScreenSize(width, height);
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
#if DX_ENGINE == 1
	// Get the render data for the shader
	engn::rend::RenderData renderData{
		m_timer->getSecondsSinceStart(),
		m_window->getWidth(),
		m_window->getHeight(),
		1.0f / m_window->getWidth(),
		1.0f / m_window->getHeight(),
	};
	// Render fucntions
	m_window->clear(BG_COLOR);
	m_engine->render(renderData);
	m_window->present();
#else
	m_scene->render(m_window->getWindowData(), m_camera, m_executor);
#endif
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

void Application::m_captureInput(MSG* mptr)
{

	// Capture and release LMB
	if (mptr->message == WM_LBUTTONDOWN) {
		engn::inp::Mouse::getInstance().onLMBPressed(mptr);
	}
	else if (mptr->message == WM_LBUTTONUP) {
		engn::inp::Mouse::getInstance().onLMBReleased(mptr);
	}
	if (mptr->message == WM_RBUTTONDOWN) {
		engn::inp::Mouse::getInstance().onRMBPressed(mptr);
	}
	else if (mptr->message == WM_RBUTTONUP) {
		engn::inp::Mouse::getInstance().onRMBReleased(mptr);
	}

	if (mptr->message == WM_MOUSEMOVE) {
		engn::inp::Mouse::getInstance().onMove(mptr);
	}

	// Capture press and release of keys
	if (mptr->message == WM_KEYDOWN)
	{
		engn::inp::Keyboard::getInstance().onKeyPressed(mptr);
	}
	else if (mptr->message == WM_KEYUP)
	{
		engn::inp::Keyboard::getInstance().onKeyReleased(mptr);
	}
}

glm::vec2 Application::m_processRMBInputs(const DirectX::XMINT2& mousePos) {
	DirectX::XMINT2 newPos;
	newPos = mousePos;
	newPos.y = m_window->getHeight() - mousePos.y;
	newPos.x /= static_cast<float>(BUFF_DECREASE_TIMES);
	newPos.y /= static_cast<float>(BUFF_DECREASE_TIMES);
	return glm::vec2{ newPos.x, newPos.y };
}

void Application::m_findObject() {
	if (!m_objectBinded && (engn::inp::Mouse::getInstance().isRMBPressed())) {
		if (m_scene->findDraggable(m_processRMBInputs(engn::inp::Mouse::getInstance().getMoveData().mousePos), m_camera)) {
			m_objectBinded = true;
		}
	}
}

void Application::m_moveObject() {
	if (m_objectBinded && (engn::inp::Mouse::getInstance().isRMBPressed())) {
		// second operation expensive
		DirectX::XMINT2 offset = engn::inp::Mouse::getInstance().getMoveData().mouseOffset;
		if (m_camStateChanged || (offset.x * offset.x + offset.y * offset.y) > 0) {
			m_scene->moveDraggable(m_processRMBInputs(engn::inp::Mouse::getInstance().getMoveData().mousePos), m_camera);
			m_camStateChanged = false;
		}
	}
}

void Application::m_releaseObject() {
	if (!engn::inp::Mouse::getInstance().isRMBPressed()) {
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
		if (engn::inp::Keyboard::getInstance().isKeyPressed(key))
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
		if (engn::inp::Keyboard::getInstance().isKeyPressed(key))
		{
			rotation += m_cameraRotations[key];
			if (!m_isCamRotating) { m_isCamRotating = true; }
		}
	}

	// pitch and yaw
	DirectX::XMINT2& offset = engn::inp::Mouse::getInstance().getMoveData().mouseOffset;
	if (engn::inp::Mouse::getInstance().isLMBPressed() && (offset.x * offset.x + offset.y * offset.y) > 0) {
		if (!m_isCamRotating) { m_isCamRotating = true; }
		float lastFPSCount = m_timer->getFPSCurrent();
		// hardcoded for now
		rotation.y += offset.x / (m_screenWidth / 2.0f) * -180.0f / 10.0f;
		rotation.x += offset.y / (m_screenHeight / 2.0f) * -180.0f / 10.0f;
		engn::inp::Mouse::getInstance().getMoveData().mouseOffset = DirectX::XMINT2{ 0, 0 };
	}

	return rotation;
}
