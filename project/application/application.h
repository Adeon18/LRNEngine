#pragma once

#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>

#include <windows.h>
#include <windowsx.h>

#include "source/math/geometry/sphere.h"

#include "render/Scene.h"

const int WM_KEY_A = 0x41;
const int WM_KEY_D = 0x44;
const int WM_KEY_W = 0x57;
const int WM_KEY_S = 0x53;

class Application
{
public:
	Application();
	//! Capture the input in sctuctures
	void captureInput(MSG* mptr);
	//! Run and render the application
	void run(const WindowRenderData& winData);

private:
	std::unordered_map<int, bool> m_pressedInputs;
	std::unordered_map<int, glm::vec3> m_directions{
		{WM_KEY_A, glm::vec3{-1, 0, 0}},
		{WM_KEY_D, glm::vec3{1, 0, 0}},
		{WM_KEY_W, glm::vec3{0, 1, 0}},
		{WM_KEY_S, glm::vec3{0, -1, 0}},
	};

	std::unique_ptr<Scene> m_scene;
	//! Move the red sphere
	void m_moveSphere(sphere& s);
};