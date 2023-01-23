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
			DirectX::XMINT2 newMosPos = DirectX::XMINT2{ GET_X_LPARAM(m->lParam), GET_Y_LPARAM(m->lParam) };
			m_moveData.mouseOffset.x = newMosPos.x - m_moveData.mousePos.x;
			m_moveData.mouseOffset.y = newMosPos.y - m_moveData.mousePos.y;
			m_moveData.mousePos = newMosPos;
		}
	} // inp
} // engn