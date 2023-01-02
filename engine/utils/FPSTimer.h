#pragma once

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace engn {

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
	{

	}

	//! Check if frame passed and return true if it did
	bool frameElapsed() {
		const auto current_time = clock::now();
		m_dt = current_time - m_base;
		m_base = current_time;

		m_lag += m_dt;

		if (m_lag > m_fixed_dt) {
			++m_ActualFPS;
			m_lag -= m_fixed_dt;
			return true;
		}

		return false;
	}
	//! Should be called just after frameTimeElapsed to get the correct debug values
	std::pair<bool, float> isDebugFPSShow() {
		std::pair<bool, float> res;
		if (m_base - m_lastUpdated > 1s)
		{
			res.first = true;
			res.second = m_ActualFPS;

			m_lastUpdated = m_base;
			m_ActualFPS = 0;
			return res;
		}
		res.first = false;
		res.second = 0.0f;
		return res;
	}

	float getFPSCurrent() { return m_ActualFPS; }

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