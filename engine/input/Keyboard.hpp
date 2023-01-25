#pragma once

#include <unordered_map>

#include <windows.h>
#include <windowsx.h>


namespace engn {
	namespace inp {
		class Keyboard {
		public:
			struct Keys {
				static constexpr int KEY_A = 0x41;
				static constexpr int KEY_D = 0x44;
				static constexpr int KEY_W = 0x57;
				static constexpr int KEY_S = 0x53;
				static constexpr int KEY_Q = 0x51;
				static constexpr int KEY_E = 0x45;
				static constexpr int KEY_H = 0x48;
				static constexpr int KEY_J = 0x4A;
				static constexpr int KEY_K = 0x4B;
				static constexpr int KEY_CTRL = VK_CONTROL;
				static constexpr int KEY_SPACE = VK_SPACE;
			};
		public:
			//! Constructor chicanery
			static Keyboard& getInstance() {
				static Keyboard handle;
				return handle;
			}
			Keyboard(const Keyboard& other) = delete;
			Keyboard& operator=(const Keyboard& other) = delete;

			//! Log each key press via logger
			[[nodiscard]] bool isInputLogged() const { return m_logInput; }
			void setInputLogging(bool value) { m_logInput = value; }
			//! Actions on key press and release
			void onKeyPressed(MSG* m);
			void onKeyReleased(MSG* m);
			//! Check if key is pressed
			[[nodiscard]] bool isKeyPressed(int key) { return m_pressedKeys[key]; }
		private:
			Keyboard() {}

			std::unordered_map<int, bool> m_pressedKeys;
			bool m_logInput = false;
		};
	} // inp
} // engn