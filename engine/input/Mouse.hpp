#pragma once

#include <unordered_map>

#include <windows.h>
#include <windowsx.h>

#include <DirectXMath.h>

namespace engn {
	namespace inp {
		using namespace DirectX;
		class Mouse {
		public:
			struct Keys {
				static constexpr int LMB = VK_LBUTTON;
				static constexpr int RMB = VK_RBUTTON;
				static constexpr int MMB = VK_MBUTTON;
				static constexpr int MX1B = VK_XBUTTON1;
				static constexpr int MX2B = VK_XBUTTON2;
			};

			struct MouseMoveData {
				XMVECTOR mousePos;
				XMVECTOR mouseOffset;
			};
		public:
			//! Constructor chicanery
			static Mouse& getInstance() {
				static Mouse handle;
				return handle;
			}
			Mouse(const Mouse& other) = delete;
			Mouse& operator=(const Mouse& other) = delete;

			//! Log each key press via logger
			[[nodiscard]] bool isInputLogged() const { return m_logInput; }
			void setInputLogging(bool value) { m_logInput = value; }

			//! Get move data by ref
			[[nodiscard]] MouseMoveData& getMoveData() { return m_moveData; }

			[[nodiscard]] bool isKeyPressed(int key) { return m_pressedButtons[key]; }
			// LMB
			void onLMBPressed(MSG* m);
			[[nodiscard]] bool isLMBPressed() { return m_pressedButtons[Keys::LMB]; }
			void onLMBReleased(MSG* m);
			// RMB
			void onRMBPressed(MSG* m);
			[[nodiscard]] bool isRMBPressed() { return m_pressedButtons[Keys::RMB]; }
			void onRMBReleased(MSG* m);
			// Move
			void onMove(MSG* m);

			// For now only 2 buttons and movement
		private:
			Mouse(){}

			std::unordered_map<int, bool> m_pressedButtons;
			MouseMoveData m_moveData;
			bool m_logInput = false;
		};
	} // inp
} // engn