#include "Keyboard.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace inp {
		void Keyboard::onKeyPressed(MSG* m) {
			m_pressedKeys[m->wParam] = true;
			if (m_logInput) { Logger::instance().logDebug("Keyboard: Key Pressed", m->wParam); }
		}
		void Keyboard::onKeyReleased(MSG* m) {
			m_pressedKeys[m->wParam] = false;
			if (m_logInput) { Logger::instance().logDebug("Keyboard: Key Released", m->wParam); }
		}
	} // inp
} // engn