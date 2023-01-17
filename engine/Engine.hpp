#pragma once

#include "render/D3D/d3d.hpp"
#include "render/Graphics/Graphics.hpp"

namespace engn {
	class Engine
	{
	public:
		Engine() {
			m_graphics.init();
		}

		// called from main.cpp
		static void init()
		{
			// initilizes engine singletons
			
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