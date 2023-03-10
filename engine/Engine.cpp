#include "Engine.hpp"

namespace engn {
	Engine::Engine() :
		m_camera{ new rend::EngineCamera{60.0f, WIN_WIDTH_DEF, WIN_HEIGHT_DEF, {0.0f, 0.0f, -2.0f}} },
		m_window{ new win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF>() }
	{
		m_graphics.init();
	}

	void Engine::setWindowSize(int screenWidth, int screenHeight) {
		m_camera->setNewScreenSize(screenWidth, screenHeight);
	}

	void Engine::setEngineData(const rend::RenderData& data)
	{
		m_renderData.iTime = data.iTime;
		m_renderData.iResolutionX = m_window->getWidth();
		m_renderData.iResolutionY = m_window->getHeight();
		m_renderData.invResolutionX = 1.0f / m_window->getWidth();
		m_renderData.invResolutionY = 1.0f / m_window->getHeight();
	}

	void Engine::render() {
		if (m_window->pollResize()) {
			setWindowSize(m_window->getWidth(), m_window->getHeight());
		}
		m_window->bindAndClearBackbuffer(BG_COLOR);
		m_graphics.renderFrame(m_camera, m_renderData, m_renderFlags);
		m_window->present();
	}

	void Engine::handlePhysics() {
		handleCameraRotation();
		handleCameraMovement();

		handleDragging();

		fillRenderModeFlagsFromInput();
	}

	void Engine::handleCameraMovement() {
		bool cameraMoved = false;

		XMVECTOR position{ 0.0f, 0.0f, 0.0f, 0.0f };
		for (const auto& key : rend::EngineCamera::CameraSettings::MOVE_KEYS) {
			if (inp::Keyboard::getInstance().isKeyPressed(key)) {
				if (!cameraMoved) { cameraMoved = true; }
				position += rend::EngineCamera::CameraSettings::MOVE_TO_ACTION[key];
			}
		}
		if (cameraMoved) {
			position = XMVector3Normalize(position);
			m_camera->addRelativeOffset(position * rend::EngineCamera::CameraSettings::CAMERA_SPEED);
		}
	}

	void Engine::handleCameraRotation() {
		bool cameraRotated = false;
		XMVECTOR rotation{ 0.0f, 0.0f, 0.0f };

		// pitch and yaw
		auto& mouse = inp::Mouse::getInstance();
		XMVECTOR& offset = mouse.getMoveData().mouseOffset;
		if (mouse.isLMBPressed() && XMVectorGetX(XMVector2LengthSq(offset)) > 0.0f) {
			if (!cameraRotated) { cameraRotated = true; }
			rotation = XMVectorSetX(rotation, XMVectorGetY(offset));
			rotation = XMVectorSetY(rotation, XMVectorGetX(offset));
			mouse.getMoveData().mouseOffset = XMVECTOR{ 0, 0 };
		}

		if (cameraRotated) {
			rotation = XMVector3Normalize(rotation);
			m_camera->addWorldRotationMat(rotation * rend::EngineCamera::CameraSettings::ROTATION_SPEED);
		}
	}

	void Engine::handleDragging() {
		findDraggable();
		moveDraggable();
	}

	void Engine::findDraggable() {
		auto& mouse = inp::Mouse::getInstance();

		if (mouse.isRMBPressed() && !m_dragger.isMeshCaptured()) {
			m_dragger.capture(m_camera);
		}
		if (!mouse.isRMBPressed()) {
			m_dragger.release();
		}
	}

	void Engine::moveDraggable() {
		if (m_dragger.isMeshCaptured()) {
			m_dragger.drag(m_camera);
		}
	}
	void Engine::fillRenderModeFlagsFromInput()
	{
		if (inp::Keyboard::getInstance().isKeyJustPressed(inp::Keyboard::Keys::KEY_N)) {
			m_renderFlags.renderFaceNormals = !m_renderFlags.renderFaceNormals;
		}

		if (inp::Keyboard::getInstance().isKeyJustPressed(inp::Keyboard::Keys::KEY_H)) {
			m_renderFlags.renderWireframes = !m_renderFlags.renderWireframes;
		}

		if (inp::Keyboard::getInstance().isKeyJustPressed(inp::Keyboard::Keys::KEY_F)) {
			m_renderFlags.bindFlashlight = !m_renderFlags.bindFlashlight;
		}
	}
} // engn