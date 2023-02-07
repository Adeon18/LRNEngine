#pragma once

#include "source/dragger/MeshDragger.hpp"

#include "input/Mouse.hpp"
#include "input/Keyboard.hpp"
#include "utils/Logger/Logger.hpp"

#include "render/Systems/MeshSystem.hpp"

#include "render/D3D/d3d.hpp"
#include "render/Graphics/EngineCamera.hpp"
#include "render/Graphics/Renderer.hpp"

namespace engn {
	using namespace DirectX;
	class Engine
	{
	public:
		Engine(int screenWidth, int screenHeight);
		//! Reset engine and camera window size for proper dragging
		void setWindowSize(int screenWidth, int screenHeight);
		//! Call the render be the rendere
		void render(const rend::RenderData& data);
		//! Handle "physics" like dragging and camera movement
		void handlePhysics(const rend::RenderData& data);

		// called from main.cpp - Must be called BEFORE engine construction
		static void init()
		{
			// initilizes engine singletons(Some, like Mouse, Keyboard, ModelManager, don't need initialization and are inited at first call)
			Logger::initConsoleLogger();
			Logger::createFileLogger("Engine", "logs/engine.log");
			Logger::instance().setDefaultLoggerName("Engine");

			rend::D3D::getInstance().init();
			rend::MeshSystem::getInstance().init();
		}

		static void deinit()
		{
			// deinitilizes engine singletons in reverse order
			rend::D3D::getInstance().deinit();
		}
	private:
		// Render
		std::unique_ptr<rend::EngineCamera> m_camera;
		rend::Renderer m_graphics;
		drag::MeshDragger m_dragger;

	private:
		//! Check for camera movement and if it exists, update it
		void handleCameraMovement();
		//! check for camera rotation and update if there was
		void handleCameraRotation();
		//! handle the entire dragging process
		void handleDragging(const rend::RenderData& data);
		//! Find the object we can capture
		void findDraggable(const rend::RenderData& data);
		//! Move the captured object if there is any	
		void moveDraggable();
	};
} // engn