#pragma once

#include "source/dragger/MeshDragger.hpp"

#include "input/Mouse.hpp"
#include "input/Keyboard.hpp"
#include "utils/Logger/Logger.hpp"

#include "render/Systems/MeshSystem.hpp"
#include "render/Systems/LightSystem.hpp"

#include "render/Graphics/HelperStructs.hpp"

#include "render/D3D/d3d.hpp"
#include "render/Graphics/EngineCamera.hpp"
#include "windows/Window.h"
#include "render/Graphics/Renderer.hpp"

#include "include/config.hpp"
#include <render/Graphics/HelperStructs.hpp>

namespace engn {
	using namespace DirectX;
	class Engine
	{
	public:
		Engine();
		//! Reset engine and camera window size for proper dragging
		void setWindowSize(int screenWidth, int screenHeight);

		//! Set the needed engine data to the renderdata struct
		void setEngineData(const rend::RenderData& data);

		//! Set all the meshes and ligts that create the initial scene
		void initScene();

		//! Run engine: Handle physics and render - return false at faulty render or cubemap baking
		bool run();

		// called from main.cpp - Must be called BEFORE engine construction
		static void init()
		{
			// initilizes engine singletons(Some, like Mouse, Keyboard, ModelManager, don't need initialization and are inited at first call)
			Logger::initConsoleLogger();
			Logger::createFileLogger("Engine", "logs/engine.log");
			Logger::instance().setDefaultLoggerName("Engine");

			rend::D3D::getInstance().init();
			rend::MeshSystem::getInstance().init();
			rend::LightSystem::getInstance().init();
		}

		static void deinit()
		{
			rend::UI::instance().deinit();
			// deinitilizes engine singletons in reverse order
			rend::D3D::getInstance().deinit();
		}
	private:
		// Render
		std::unique_ptr<rend::EngineCamera> m_camera;
		std::unique_ptr<win::Window<WIN_WIDTH_DEF, WIN_HEIGHT_DEF>> m_window;
		rend::Renderer m_graphics;
		drag::MeshDragger m_dragger;

		rend::RenderData m_renderData;
		rend::RenderModeFlags m_renderFlags;

	private:
		//! Call the render be the rendere
		bool render();
		//! Handle "physics" like dragging and camera movement
		void handlePhysics();
		//! Check for camera movement and if it exists, update it
		void handleCameraMovement();
		//! check for camera rotation and update if there was
		void handleCameraRotation();
		//! handle the entire dragging process
		void handleDragging();
		//! Find the object we can capture
		void findDraggable();
		//! Move the captured object if there is any	
		void moveDraggable();
		//! Based on the keyboard inputs, fill the struct that has info what mode of rendering is used(debug/normls/wireframs, etc.)
		void fillRenderModeFlagsFromInput();

#ifdef _WIN64
		const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\..\\";
#else
		const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\";
#endif

		std::unordered_map<std::string, rend::MaterialTexturePaths> MATERIALS{
				{"STONE",
					{
						TEX_REL_PATH_PREF + "assets\\Textures\\Stone\\Stone_COLOR.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Stone\\Stone_NORM.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Stone\\Stone_ROUGH.dds",
						""
					}
				},
				{"COBBLESTONE",
					{
						TEX_REL_PATH_PREF + "assets\\Textures\\Cobblestone\\Cobblestone_albedo.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Cobblestone\\Cobblestone_normal.dds",
						TEX_REL_PATH_PREF + "assets\\Textures\\Cobblestone\\Cobblestone_roughness.dds",
						""
					}
				},
				{"TEST",
					{
						TEX_REL_PATH_PREF + "assets\\Textures\\BrickWall\\blank.dds",
						"",
						"",
						""
					}
				},
		};

		std::unordered_map<std::string, std::string> MODELS{
			{"HORSE", TEX_REL_PATH_PREF + "assets/Models/KnightHorse/KnightHorse.fbx"},
			{"SAMURAI", TEX_REL_PATH_PREF + "assets/Models/Samurai/Samurai.fbx"},
			{"TOWER", TEX_REL_PATH_PREF + "assets/Models/EastTower/EastTower.fbx"},
			{"CUBE", TEX_REL_PATH_PREF + "assets/Models/Cube/Cube.fbx"},
			{"SPHERE", TEX_REL_PATH_PREF + "assets/Models/Sphere/sphere.fbx"},
		};
	};
} // engn