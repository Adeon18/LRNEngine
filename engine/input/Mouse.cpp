#include "Mouse.hpp"


namespace engn {
	namespace inp {
		void Mouse::onLMBPressed(MSG* m)
		{
			m_pressedButtons[Keys::LMB] = true;
		}
		void Mouse::onLMBReleased(MSG* m)
		{
			m_pressedButtons[Keys::LMB] = false;
		}
		void Mouse::onRMBPressed(MSG* m)
		{
			m_pressedButtons[Keys::RMB] = true;
		}
		void Mouse::onRMBReleased(MSG* m)
		{
			m_pressedButtons[Keys::RMB] = false;
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