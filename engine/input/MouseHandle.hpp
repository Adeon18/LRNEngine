#pragma once

#include <unordered_map>

#include <windows.h>
#include <windowsx.h>

#include <DirectXMath.h>

namespace engn {
	namespace inp {
		class MouseHandle {
			struct Keys {
				static constexpr int LMB = VK_LBUTTON;
				static constexpr int RMB = VK_RBUTTON;
				static constexpr int MMB = VK_MBUTTON;
				static constexpr int MX1B = VK_XBUTTON1;
				static constexpr int MX2B = VK_XBUTTON2;
			};

			struct MouseMoveData {
				DirectX::XMINT2 mousePos;
				DirectX::XMINT2 mouseOffset;
			};
		public:
			static MouseHandle& getInstance() {
				static MouseHandle handle;
				return handle;
			}
			MouseHandle(const MouseHandle& other) = delete;
			MouseHandle& operator=(const MouseHandle& other) = delete;

			[[nodiscard]] bool isInputLogged() { return logInput; }
			void setInputLogging(bool value) { logInput = value; }

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
			MouseHandle(){}

			std::unordered_map<int, bool> m_pressedButtons;
			MouseMoveData m_moveData;
			bool logInput = false;
		};
	} // inp
} // engn