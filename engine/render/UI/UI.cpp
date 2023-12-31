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
			if (ImGui::CollapsingHeader("Shadows")) {
				manageShadows();
			}
			if (ImGui::CollapsingHeader("Spawning")) {
				manageSpawn();
			}
			if (ImGui::CollapsingHeader("Particles")) {
				manageParticles();
			}
			if (ImGui::CollapsingHeader("Anti-Aliasing")) {
				manageAA();
			}
			if (ImGui::CollapsingHeader("Bloom")) {
				manageBloom();
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
		void UI::manageShadows()
		{
			ImGui::Checkbox("Enable Shadows", &m_shadowData.enabled);
			ImGui::SliderFloat("Directional Bias Max", &m_shadowData.directionalBiasMax, 0.0001f, 0.01f, "%.5f");
			ImGui::SliderFloat("Directional Bias Min", &m_shadowData.directionalBiasMin, 0.00001f, 0.001f, "%.5f");
			ImGui::SliderFloat("Point Bias Max", &m_shadowData.pointBiasMax, 0.001f, 0.01f, "%.5f");
			ImGui::SliderFloat("Point Bias Min", &m_shadowData.pointBiasMin, 0.00001f, 0.001f, "%.5f");
			ImGui::SliderFloat("Spot Bias Max", &m_shadowData.spotBiasMax, 0.0001f, 0.01f, "%.5f");
			ImGui::SliderFloat("Spot Bias Min", &m_shadowData.spotBiasMin, 0.00001f, 0.001f, "%.5f");
		}
		void UI::manageSpawn()
		{
			ImGui::Combo("Model", &m_spawnData.modelToSpawnIdx, MODELS, ARRAYSIZE(MODELS));
			ImGui::SliderFloat("Distance", &m_spawnData.spawnDistance, 2.0f, 10.0f);
			ImGui::SliderFloat("Spawn TIme", &m_spawnData.modelSpawnTime, 0.2f, 5.0f);
		}
		void UI::manageParticles()
		{
			ImGui::SliderInt("Animation FPS", &m_particleData.animationFPS, 4, 10);
		}
		void UI::manageAA()
		{
			ImGui::Checkbox("Enable", &m_AAdata.enabled);
			ImGui::SliderFloat("qualitySubpix", &m_AAdata.qualitySubpix, 0.0f, 1.0f);
			ImGui::SliderFloat("qualityEdgeThreshold", &m_AAdata.qualityEdgeThreshold, 0.063f, 0.333f);
			ImGui::SliderFloat("qualityEdgeThresholdMin", &m_AAdata.qualityEdgeThresholdMin, 0.0312f, 0.0833f, "%.4f");
		}
		void UI::manageBloom()
		{
			ImGui::Checkbox("Enable", &m_bloomData.enabled);
		}
	}
} // engn