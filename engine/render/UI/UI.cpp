#include "UI.hpp"

namespace engn {
	namespace rend {
		void UI::init(HWND hWnd)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			ImGui_ImplWin32_Init(hWnd);
			ImGui_ImplDX11_Init(d3d::s_device, d3d::s_devcon);
			ImGui::StyleColorsDark();
		}
		void UI::startFrame()
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("Debug Window");
		}
		void UI::manageMenus()
		{
			if (ImGui::CollapsingHeader("Geometry")) {
				manageGeometry();
			}
			if (ImGui::CollapsingHeader("Material")) {
				manageMaterial();
			}
			if (ImGui::CollapsingHeader("Lighting")) {
				manageLighting();
			}
		}
		void UI::endFrame()
		{
			ImGui::End();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		void UI::deinit()
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}
		void UI::manageGeometry()
		{
			ImGui::Checkbox("Visualize normals", &m_geometryData.normalVisEnabled);
			ImGui::Checkbox("Visualize vireframes", &m_geometryData.vireframeVisEnabled);
		}
		void UI::manageMaterial()
		{
		}
		void UI::manageLighting()
		{
		}
	}
} // engn