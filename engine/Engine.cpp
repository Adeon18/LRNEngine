#include "Engine.hpp"

namespace engn {
	Engine::Engine(int screenWidth, int screenHeight) :
		m_camera{ new rend::EngineCamera{45.0f, screenWidth, screenHeight, {0.0f, 0.0f, -2.0f}} }
	{
		m_graphics.init();
	}

	void Engine::setWindowSize(int screenWidth, int screenHeight) {
		m_camera->setNewScreenSize(screenWidth, screenHeight);
	}

	void Engine::render(const rend::RenderData& data) {
		m_graphics.renderFrame(m_camera, data, m_renderFlags);
	}

	void Engine::handlePhysics(const rend::RenderData& data) {
		handleCameraRotation();
		handleCameraMovement();

		handleDragging(data);

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

	void Engine::handleDragging(const rend::RenderData& data) {
		findDraggable(data);
		moveDraggable();
	}

	void Engine::findDraggable(const rend::RenderData& data) {
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

		if (inp::Keyboard::getInstance().isKeyJustPressed(inp::Keyboard::Keys::KEY_F)) {
			m_renderFlags.renderWireframes = !m_renderFlags.renderWireframes;
		}
	}
} // engn