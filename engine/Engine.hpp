#pragma once

#include "input/Mouse.hpp"
#include "input/Keyboard.hpp"
#include "utils/Logger/Logger.hpp"

#include "render/Systems/MeshSystem.hpp"

#include "render/D3D/d3d.hpp"
#include "render/Graphics/EngineCamera.hpp"
#include "render/Graphics/Graphics.hpp"

namespace engn {
	using namespace DirectX;
	class Engine
	{
	public:
		struct CameraSettings {
			static constexpr float CAMERA_SPEED = 0.02f;
			static constexpr float ROTATION_SPEED = 1.2f;
;			inline static const std::array<int, 6> MOVE_KEYS{
				inp::Keyboard::Keys::KEY_A,
				inp::Keyboard::Keys::KEY_D,
				inp::Keyboard::Keys::KEY_W,
				inp::Keyboard::Keys::KEY_S,
				inp::Keyboard::Keys::KEY_SPACE,
				inp::Keyboard::Keys::KEY_CTRL,
			};
			inline static const std::array<int, 2> ROLL_KEYS{
				inp::Keyboard::Keys::KEY_Q,
				inp::Keyboard::Keys::KEY_E,
			};
			inline static std::unordered_map<int, XMVECTOR> MOVE_TO_ACTION {
				{inp::Keyboard::Keys::KEY_A, {-1.0f, 0.0f, 0.0f, 0.0f}},
				{inp::Keyboard::Keys::KEY_D, {1.0f, 0.0f, 0.0f, 0.0f}},
				{inp::Keyboard::Keys::KEY_CTRL, {0.0f, -1.0f, 0.0f, 0.0f}},
				{inp::Keyboard::Keys::KEY_SPACE, {0.0f, 1.0f, 0.0f, 0.0f}},
				{inp::Keyboard::Keys::KEY_W, {0.0f, 0.0f, 1.0f, 0.0f}},
				{inp::Keyboard::Keys::KEY_S, {0.0f, 0.0f, -1.0f, 0.0f}},
				// rotation
				{inp::Keyboard::Keys::KEY_Q, {0.0f, 0.0f, 1.0f, 0.0f}},
				{inp::Keyboard::Keys::KEY_E, {0.0f, 0.0f, -1.0f, 0.0f}},
			};
		};
	public:
		Engine(int screenWidth, int screenHeight):
			m_camera{ new rend::EngineCamera{45.0f, screenWidth, screenHeight, {0.0f, 0.0f, -2.0f}} }
		{
			m_graphics.init();
		}

		void render(const rend::RenderData& data) {
			m_graphics.renderFrame(m_camera, data);
		}

		void handlePhysics(const rend::RenderData& data) {
			handleCameraRotation();
			handleCameraMovement();

			handleDragging(data);
		}

		// called from main.cpp - Must be called BEFORE engine construction
		static void init()
		{
			// initilizes engine singletons
			Logger::initConsoleLogger();
			Logger::createFileLogger("Engine", "engine.log");
			Logger::instance().setDefaultLoggerName("Engine");

			m_d3d.init();
			rend::MeshSystem::getInstance().init();
		}

		static void deinit()
		{
			// deinitilizes engine singletons in reverse order
			m_d3d.deinit();
		}
	private:
		// Singletons
		static inline rend::D3D& m_d3d = rend::D3D::getInstance();
		static inline inp::Mouse& m_mouse = inp::Mouse::getInstance();
		static inline inp::Keyboard& m_keyboard = inp::Keyboard::getInstance();
		// Render
		std::unique_ptr<rend::EngineCamera> m_camera;
		rend::Graphics m_graphics;

	private:
		//! Check for camera movement and if it exists, update it
		void handleCameraMovement() {
			bool cameraMoved = false;
			
			XMVECTOR position{0.0f, 0.0f, 0.0f, 0.0f};
			for (const auto& key : CameraSettings::MOVE_KEYS) {
				if (m_keyboard.isKeyPressed(key)) {
					if (!cameraMoved) { cameraMoved = true; }
					position += CameraSettings::MOVE_TO_ACTION[key];
				}
			}
			if (cameraMoved) {
				position = XMVector3Normalize(position);
				m_camera->addRelativeOffset(position * CameraSettings::CAMERA_SPEED);
			}
		}
		//! check for camera rotation and update if there was
		void handleCameraRotation() {
			bool cameraRotated = false;
			XMVECTOR rotation{ 0.0f, 0.0f, 0.0f };
			// Roll
			for (const auto& key : CameraSettings::ROLL_KEYS) {
				if (m_keyboard.isKeyPressed(key)) {
					if (!cameraRotated) { cameraRotated = true; }
					rotation += CameraSettings::MOVE_TO_ACTION[key];
				}
			}
			// pitch and yaw
			XMVECTOR& offset = m_mouse.getMoveData().mouseOffset;
			if (m_mouse.isLMBPressed() && XMVectorGetX(XMVector2LengthSq(offset)) > 0.0f) {
				if (!cameraRotated) { cameraRotated = true; }
				rotation = XMVectorSetX(rotation, XMVectorGetY(offset));
				rotation = XMVectorSetY(rotation, XMVectorGetX(offset));
				m_mouse.getMoveData().mouseOffset = XMVECTOR{ 0, 0 };
			}

			if (cameraRotated) {
				rotation = XMVector3Normalize(rotation);
				m_camera->addRelativeRotationQuat(rotation * CameraSettings::ROTATION_SPEED);
			}
		}

		void handleDragging(const rend::RenderData& data) {
			findDraggable(data);
			moveDraggable();
		}

		void findDraggable(const rend::RenderData& data) {
			if (m_mouse.isRMBPressed()) {

				XMVECTOR viewingFrustumNearPlane[4] =
				{
					{-1.0f, -1.0f, 0.0f, 1.0f},
					{-1.0f,  1.0f, 0.0f, 1.0f},
					{ 1.0f,  1.0f, 0.0f, 1.0f},
					{ 1.0f, -1.0f, 0.0f, 1.0f},
				};
				XMVECTOR viewingFrustumNearPlaneWorldSpace[4];

				XMMATRIX viewProjInv = XMMatrixInverse(nullptr, m_camera->getViewMatrix() * m_camera->getProjMatrixReversed());
				for (uint32_t i = 0; i < 4; ++i) {
					viewingFrustumNearPlaneWorldSpace[i] = XMVector3Transform(viewingFrustumNearPlane[i], viewProjInv);
					viewingFrustumNearPlaneWorldSpace[i] /= XMVectorGetW(viewingFrustumNearPlaneWorldSpace[i]);
				}
				XMVECTOR BLPlanePos = viewingFrustumNearPlaneWorldSpace[0];
				XMVECTOR BLToTL = XMVector3Normalize(viewingFrustumNearPlaneWorldSpace[1] - BLPlanePos);
				XMVECTOR BLToBR = XMVector3Normalize(viewingFrustumNearPlaneWorldSpace[3] - BLPlanePos);
				std::cout << "BLPlanePos: " << BLPlanePos << " BLToTL: " << BLToTL << " BLToBR: " << BLToBR << std::endl;

				float nearPlaneWidth = XMVectorGetX(viewingFrustumNearPlaneWorldSpace[3] - viewingFrustumNearPlaneWorldSpace[0]);
				float nearPlaneHeight = XMVectorGetY(viewingFrustumNearPlaneWorldSpace[2] - viewingFrustumNearPlaneWorldSpace[3]);

				float pixelWidth = nearPlaneWidth / data.iResolutionX;
				float pixelHeight = nearPlaneHeight / data.iResolutionY;

				std::cout << "ScreenWidth: " << nearPlaneWidth << " ScreenHeight: " << nearPlaneHeight << std::endl;
				std::cout << "PixelWidth: " << pixelWidth << " PixelHeight: " << pixelHeight << std::endl;

				XMVECTOR rayPos = m_camera->getCamPosition();
				XMVECTOR rayTo = BLPlanePos +
					BLToTL * pixelHeight * data.iResolutionY / 4.0f +
					BLToBR * pixelWidth * data.iResolutionX / 2.0f;
				XMVECTOR rayDir = XMVector3Normalize(rayTo - rayPos);
				std::cout << "RayPos: " << rayPos << " RayTo: " << rayTo << " RayDir: " << rayDir << std::endl;

				//geom::Ray ray{m_camera->getCamPosition(), m_get}
			}
		}

		void moveDraggable() {

		}
	};
} // engn