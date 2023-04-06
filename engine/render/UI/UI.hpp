#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {
		class UI {
		public:
			static UI& instance() {
				static UI ui;
				return ui;
			}
			UI(const UI& ui) = delete;
			UI& operator=(const UI& ui) = delete;

			//! Initialize UI based on hWnd => should be initialize in Window class
			void init(HWND hWnd);
			//! Start the imgui frame, should be done before Rendered::renderFrame
			void startFrame();
		private:
			UI() {}
		};
	} // rend
} // engn