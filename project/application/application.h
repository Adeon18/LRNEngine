#pragma once

#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>

#include <windows.h>
#include <windowsx.h>

#include "render/Camera/Camera.h"

#include "source/math/geometry/sphere.h"

#include "render/Scene/Scene.h"

class Application
{
	static struct Keys {
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
	};
public:
	Application(int width, int height);
	//! Capture the input in sctuctures
	void captureInput(MSG* mptr);
	//! Run and render the application
	void run(const WindowRenderData& winData);

private:
	int m_screenWidth;
	int m_screenHeight;

	glm::vec2 m_mousePos;
	glm::vec2 m_mouseOffset;

	std::unordered_map<int, bool> m_pressedInputs;

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
		{Keys::KEY_E, glm::vec3(0.0f, 0.0f, -10.0f)},
		{Keys::KEY_Q, glm::vec3(0.0f, 0.0f, 10.0f)},
	};

	std::unique_ptr<Scene> m_scene;
	std::unique_ptr<Camera> m_camera;
	//! Move the red sphere
	void m_moveCamera();

	glm::vec3 getRotation();
};