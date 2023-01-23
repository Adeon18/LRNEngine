#include "MouseHandle.hpp"


namespace engn {
	namespace inp {
		void MouseHandle::onLMBPressed(MSG* m)
		{
			m_pressedButtons[Keys::LMB] = true;
		}
		void MouseHandle::onLMBReleased(MSG* m)
		{
			m_pressedButtons[Keys::LMB] = false;
		}
		void MouseHandle::onRMBPressed(MSG* m)
		{
			m_pressedButtons[Keys::RMB] = true;
		}
		void MouseHandle::onRMBReleased(MSG* m)
		{
			m_pressedButtons[Keys::RMB] = false;
		}
		void MouseHandle::onMove(MSG* m)
		{
			DirectX::XMINT2 newMosPos = DirectX::XMINT2{ GET_X_LPARAM(m->lParam), GET_Y_LPARAM(m->lParam) };
			m_moveData.mouseOffset.x = newMosPos.x - m_moveData.mousePos.x;
			m_moveData.mouseOffset.y = newMosPos.y - m_moveData.mousePos.y;
			m_moveData.mousePos = newMosPos;
		}
	} // inp
} // engn