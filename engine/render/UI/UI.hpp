#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "render/D3D/d3d.hpp"

namespace engn {
	namespace rend {
		class UI {
			struct GeometryWidgetData {
				bool normalVisEnabled = false;
				bool vireframeVisEnabled = false;

				//! Check if any of the debug visualization is active
				[[nodiscard]] bool isDebugVisEnabled() const { return normalVisEnabled || vireframeVisEnabled; }
			};
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

			void manageMenus();
			//! End the imgui frame render
			void endFrame();
			//! To be called by Engine on exit, frees all imgui data
			void deinit();

			[[nodiscard]] const GeometryWidgetData& getGeomWidgetData() const { return m_geometryData; }
		private:
			UI() {}

			//! Respective widget managers
			void manageGeometry();
			void manageMaterial();
			void manageLighting();

			GeometryWidgetData m_geometryData;
		};
	} // rend
} // engn