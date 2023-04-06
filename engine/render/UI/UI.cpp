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
	}
} // engn