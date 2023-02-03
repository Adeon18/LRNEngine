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
			if (m_mouse.isRMBPressed() && !foundDraggable) {
				auto& mPos = m_mouse.getMoveData();

				geom::Ray atMouse = m_camera->castRay(XMVectorGetX(mPos.mousePos), XMVectorGetY(mPos.mousePos));

				mdl::MeshIntersection closest{ {}, {}, 1000.0f, 0 };
				auto collisionRes = rend::MeshSystem::getInstance().getClosestNormalMesh(atMouse, closest);
				std::cout << "Collision Happened: " << collisionRes.first << std::endl;
				std::cout << "Collision Pos: " << closest.pos << std::endl;
				std::cout << "Collision T: " << closest.t << std::endl;
				std::cout << "Collision Trinagle Idx: " << closest.triangle << std::endl;
				foundDraggable = true;
			}
			if (!m_mouse.isRMBPressed()) {
				foundDraggable = false;
			}
		}

		void moveDraggable() {

		}

		bool foundDraggable = false;
	};
} // engn