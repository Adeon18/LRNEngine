#include <iostream>

#include "Mouse.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace inp {
		void Mouse::onLMBPressed(MSG* m)
		{
			m_pressedButtons[Keys::LMB] = true;
			if (m_logInput) {
				Logger::instance().logDebug("Mouse: LMB Pressed");
			}
		}
		void Mouse::onLMBReleased(MSG* m)
		{
			m_pressedButtons[Keys::LMB] = false;
			if (m_logInput) { Logger::instance().logDebug("Mouse: LMB Released"); }
		}
		void Mouse::onRMBPressed(MSG* m)
		{
			m_pressedButtons[Keys::RMB] = true;
			if (m_logInput) { Logger::instance().logDebug("Mouse: RMB Pressed"); }
		}
		void Mouse::onRMBReleased(MSG* m)
		{
			m_pressedButtons[Keys::RMB] = false;
			if (m_logInput) { Logger::instance().logDebug("Mouse: RMB Released"); }
		}
		void Mouse::onMove(MSG* m)
		{
			XMVECTOR newMosPos = XMVECTOR{ static_cast<float>(GET_X_LPARAM(m->lParam)), static_cast<float>((GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYEDGE) * 2) + GET_Y_LPARAM(m->lParam)) };
			m_moveData.mouseOffset = newMosPos - m_moveData.mousePos;
			m_moveData.mousePos = newMosPos;
		}
	} // inp
} // engnd