#pragma once

#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>

#include <windows.h>
#include <windowsx.h>

#include "source/math/geometry/sphere.h"

const int WM_KEY_A = 0x41;
const int WM_KEY_D = 0x44;
const int WM_KEY_W = 0x57;
const int WM_KEY_S = 0x53;

class Application
{
public:
	void captureInput(MSG *mptr)
	{
		// RMB down
		if (mptr->message == WM_RBUTTONDOWN)
		{
			m_pressedInputs[WM_RBUTTONDOWN] = true;
		}
		// RMB UP - Clear all inputs
		if (mptr->message == WM_RBUTTONUP)
		{
			for (auto &el: m_pressedInputs)
			{
				el.second = false;
			}
		}

		if (mptr->message == WM_KEYDOWN)
		{
			switch(mptr->wParam)
			{
			case WM_KEY_A: m_pressedInputs[WM_KEY_A] = true; break;
			case WM_KEY_D: m_pressedInputs[WM_KEY_D] = true; break;
			case WM_KEY_W: m_pressedInputs[WM_KEY_W] = true; break;
			case WM_KEY_S: m_pressedInputs[WM_KEY_S] = true; break;
			}
		} else if (mptr->message == WM_KEYUP)
		{
			switch (mptr->wParam)
			{
			case WM_KEY_A: m_pressedInputs[WM_KEY_A] = false; break;
			case WM_KEY_D: m_pressedInputs[WM_KEY_D] = false; break;
			case WM_KEY_W: m_pressedInputs[WM_KEY_W] = false; break;
			case WM_KEY_S: m_pressedInputs[WM_KEY_S] = false; break;
			}
		}
	}

	void run(sphere &s)
	{
		m_moveSphere(s);
	}
private:
	std::unordered_map<int, bool> m_pressedInputs;
	std::unordered_map<int, glm::vec3> m_directions{
		{WM_KEY_A, glm::vec3{-1, 0, 0}},
		{WM_KEY_D, glm::vec3{1, 0, 0}},
		{WM_KEY_W, glm::vec3{0, 1, 0}},
		{WM_KEY_S, glm::vec3{0, -1, 0}},
	};

	void m_moveSphere(sphere &s)
	{
		glm::vec3 direction{ 0 };
		for (const auto& [key, pressed] : m_pressedInputs)
		{
			if (pressed)
			{
				direction += m_directions[key];
			}
		}
		glm::normalize(direction);

		s.m_center += direction * 2.0f;
	}
};