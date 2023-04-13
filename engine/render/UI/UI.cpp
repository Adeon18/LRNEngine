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
			ImGui::Checkbox("Visualize wireframes", &m_geometryData.vireframeVisEnabled);
		}
		void UI::manageMaterial()
		{
			ImGui::Checkbox("Use texture roughness", &m_materialData.useTextureRoughness);
			ImGui::SliderFloat("Roughness", &m_materialData.defaultRoughness, 0.0f, 1.0f);
			ImGui::Checkbox("Use texture metallic", &m_materialData.useTextureMetallic);
			ImGui::SliderFloat("Metallic", &m_materialData.defaultMetallic, 0.0f, 1.0f);
		}
		void UI::manageLighting()
		{
			ImGui::Checkbox("Enable Diffuse", &m_lightingData.toggleDiffuse);
			ImGui::Checkbox("Enable Specular", &m_lightingData.toggleSpecular);
			ImGui::Checkbox("Enable IBL", &m_lightingData.toggleIBL);
		}
	}
} // engn