#include "Keyboard.hpp"

namespace engn {
	namespace inp {
		void Keyboard::onKeyPressed(MSG* m) {
			m_pressedKeys[m->wParam] = true;
		}
		void Keyboard::onKeyReleased(MSG* m) {
			m_pressedKeys[m->wParam] = false;
		}
	} // inp
} // engn