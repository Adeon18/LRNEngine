#pragma once

#include "utils/Logger/Logger.hpp"

#include "render/D3D/d3d.hpp"
#include "render/Graphics/Graphics.hpp"

namespace engn {
	class Engine
	{
	public:
		Engine() {
			m_graphics.init();
		}

		void render(const rend::RenderData& data) {
			m_graphics.renderFrame(data);
		}

		// called from main.cpp - Must be called BEFORE engine construction
		static void init()
		{
			// initilizes engine singletons
			Logger::initConsoleLogger();
			Logger::createFileLogger("Engine", "logs/engine.log");
			Logger::instance().setDefaultLoggerName("Engine");
			m_d3d.init();
		}

		static void deinit()
		{
			// deinitilizes engine singletons in reverse order
			m_d3d.deinit();
		}
	private:
		static inline rend::D3D& m_d3d = rend::D3D::getInstance();

		rend::Graphics m_graphics;
	};
} // engn