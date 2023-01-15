#pragma once

#include <chrono>
#include <iostream>


namespace engn {
	using namespace std::chrono_literals;


	class FPSTimer {
		using clock = std::chrono::high_resolution_clock;
	public:
		//! Default constructor
		FPSTimer(float FPS) :
			m_base{ clock::now() },
			m_dt{ 0ns }, m_lag{ 0ns },
			m_lastUpdated{ clock::now() },
			m_FPS{ FPS }, m_ActualFPS{},
			m_fixed_dt{ static_cast<uint64_t>(1.0f / FPS * std::nano::den) }
		{}

		//! Check if frame passed and return true if it did
		bool frameElapsed();
		//! Should be called just after frameTimeElapsed to get the correct debug values
		std::pair<bool, float> isDebugFPSShow();

		float getFPSCurrent();

	private:
		std::chrono::time_point<clock> m_base;
		std::chrono::nanoseconds m_dt;

		std::chrono::nanoseconds m_lag;
		std::chrono::nanoseconds m_fixed_dt;
		float m_FPS;
		float m_ActualFPS;
		std::chrono::time_point<clock> m_lastUpdated;
	};

} // engn