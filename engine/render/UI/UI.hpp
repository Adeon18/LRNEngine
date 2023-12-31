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

			struct MaterialWidgetData {
				bool useTextureMetallic = true;
				bool useTextureRoughness = true;

				float defaultMetallic = 0.3f;
				float defaultRoughness = 0.7f;
			};

			struct LightingWidgetData {
				bool toggleDiffuse = true;
				bool toggleSpecular = true;
				bool toggleIBL = true;
			};

			struct ShadowWidgetData {
				bool enabled = true;

				float directionalBiasMax = 0.0005f;
				float directionalBiasMin = 0.00005f;

				float pointBiasMax = 0.005f;
				float pointBiasMin = 0.00005f;

				float spotBiasMax = 0.0005f;
				float spotBiasMin = 0.00005f;
			};

			struct SpawnWidgetData {
				int modelToSpawnIdx = 0;
				float spawnDistance = 5.0f;
				float modelSpawnTime = 1.0f;
			};

			struct ParticleWidgetData {
				int animationFPS = 16;
			};

			struct AAWidgetData {
				float qualitySubpix = 0.5f;
				float qualityEdgeThreshold = 0.166f;
				float qualityEdgeThresholdMin = 0.0625f;
				bool enabled = true;
			};

			struct BloomWidgetData {
				bool enabled = true;
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
			[[nodiscard]] const MaterialWidgetData& getMatWidgetData() const { return m_materialData; }
			[[nodiscard]] const LightingWidgetData& getLightWidgetData() const { return m_lightingData; }
			[[nodiscard]] const ShadowWidgetData& getShadowWidgetData() const { return m_shadowData; }
			[[nodiscard]] const SpawnWidgetData& getSpawnWidgetData() const { return m_spawnData; }
			[[nodiscard]] const ParticleWidgetData& getParticleWidgetData() const { return m_particleData; }
			[[nodiscard]] const AAWidgetData& getAAWidgetData() const { return m_AAdata; }
			[[nodiscard]] const BloomWidgetData& getBloomWidgetData() const { return m_bloomData; }

			[[nodiscard]] std::string getModelNameFromWidgetIdx(int idx) const { return MODELS[idx]; }
		private:
			UI() {}

			//! Respective widget managers
			void manageGeometry();
			void manageMaterial();
			void manageLighting();
			void manageShadows();
			void manageSpawn();
			void manageParticles();
			void manageAA();
			void manageBloom();

			GeometryWidgetData m_geometryData;
			MaterialWidgetData m_materialData;
			LightingWidgetData m_lightingData;
			ShadowWidgetData m_shadowData;
			SpawnWidgetData m_spawnData;
			ParticleWidgetData m_particleData;
			AAWidgetData m_AAdata;
			BloomWidgetData m_bloomData;

			static constexpr const char* MODELS[] = { "HORSE", "SAMURAI", "TOWER" };
		};
	} // rend
} // engn