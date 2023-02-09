#pragma once

#include <iostream>
#include <unordered_map>

#include <windows.h>
#include <windowsx.h>

#include <glm/glm/glm.hpp>

#include "Engine.hpp"

#include "include/config.hpp"

#include "utils/Timer/FPSTimer.h"
#include "utils/paralell_executor/parallel_executor.h"

#include "render/RayTracer/Scene/Scene.h"
#include "render/RayTracer/Camera/Camera.h"

#include "source/math/geometry/sphere.h"
#include "source/math/geometry/triangle.h"


class Application
{
	struct Keys {
		inline static int KEY_A = 0x41;
		inline static int KEY_D = 0x44;
		inline static int KEY_W = 0x57;
		inline static int KEY_S = 0x53;
		inline static int KEY_Q = 0x51;
		inline static int KEY_E = 0x45;
		inline static int KEY_H = 0x48;
		inline static int KEY_J = 0x4A;
		inline static int KEY_K = 0x4B;
		inline static int KEY_L = 0x4C;
		inline static int KEY_CTRL = VK_CONTROL;
		inline static int KEY_SPACE = VK_SPACE;
		inline static int LMB = VK_LBUTTON;
		inline static int RMB = VK_RBUTTON;
	};
	static constexpr int WIN_WIDTH_DEF = 960;
	static constexpr int WIN_HEIGHT_DEF = 540;
	static constexpr int BUFF_DECREASE_TIMES = 2;
	static constexpr float ROLL_SPEED_DEG = 1.0f;
	inline static float BG_COLOR[] = { 0.5f, 0.2f, 0.9f, 1.0f };

	//! FPS which the timer tries to tick at
	static constexpr float TIMER_FPS = 300.0f;
	//! Our scene default camera FOV
	static constexpr float CAMERA_FOV = 45.0f;

public:
	struct MouseInputData {
		glm::vec2 mousePos;
		glm::vec2 mouseOffset;
	};
public:
	Application();
	//! Run and render the application
	int run();

	void setWindowSize(int width, int height);
private:
	//! Process the win32 API message queue
	void m_processWIN32Queue(MSG* mptr);
	//! A function responsible for scene render
	void m_handleRender();
	// A function responsible for handling "physics" like camera movement
	void m_handlePhysics();

	//! Capture the input into a map
	void m_captureInput(MSG* mptr);

	//! Mouse Input
	glm::vec2 m_processRMBInputs(const DirectX::XMINT2& mousePos);


	//! Put objects on the scene
	void m_createObjects();

	//! Handle Object Dragging
	//! Cast ray to find if we picked something
	void m_findObject();
	//! If we picked something and we are holding RMB, make scene get the new position of the ray and move sphere there
	void m_moveObject();
	//! Release the object from dragging(releases in application, in scsne is released on RMB Press)
	void m_releaseObject();

	void m_handleDragging();

	//! Handle the camera movement
	void m_moveCamera();
	//! Get the camera rotation from processed inputs(deg.x, deg.y, deg.z)
	glm::vec3 m_getCamRotation();
	//! Get the normalized camera movement vector
	glm::vec3 m_getCamMovement();

private:
	int m_screenWidth;
	int m_screenHeight;

	bool m_isRunning;

	// Movement and game logic handling(separate for draggers and camera)
	MouseInputData m_camMoveData;
	MouseInputData m_objDragData;

	bool m_objectBinded = false;
	bool m_camStateChanged = false;

	bool m_isCamMoving = false;
	bool m_isCamRotating = false;


	std::vector<int> m_camMoveInputs{ Keys::KEY_A, Keys::KEY_D, Keys::KEY_W, Keys::KEY_S, Keys::KEY_CTRL, Keys::KEY_SPACE };
	std::unordered_map<int, glm::vec3> m_cameraDirections{
		{Keys::KEY_A, glm::vec3{-1, 0, 0}},
		{Keys::KEY_D, glm::vec3{1, 0, 0}},
		{Keys::KEY_CTRL, glm::vec3{0, -1, 0}},
		{Keys::KEY_SPACE, glm::vec3{0, 1, 0}},
		{Keys::KEY_W, glm::vec3{0, 0, -1}},
		{Keys::KEY_S, glm::vec3{0, 0, 1}},
	};

	std::vector<int> m_camRotateInputs{ Keys::KEY_E, Keys::KEY_Q };
	std::unordered_map<int, glm::vec3> m_cameraRotations{
		{Keys::KEY_E, glm::vec3(0.0f, 0.0f, -ROLL_SPEED_DEG)},
		{Keys::KEY_Q, glm::vec3(0.0f, 0.0f, ROLL_SPEED_DEG)},
	};

	// Application building blocks
	std::unique_ptr<engn::util::FPSTimer> m_timer;
	std::unique_ptr<engn::win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF, BUFF_DECREASE_TIMES>> m_window;
	// Engine
	std::unique_ptr<engn::Engine> m_engine;
	// Raytracer
	std::unique_ptr<engn::Scene> m_scene;
	std::unique_ptr<engn::Camera> m_camera;
	std::unique_ptr<engn::util::ParallelExecutor> m_executor;
};