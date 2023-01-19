#include "FPSTimer.h"

namespace engn {
	namespace util {
		bool FPSTimer::frameElapsed() {
			const auto current_time = clock::now();
			m_dt = current_time - m_base;
			m_base = current_time;

			m_lag += m_dt;

			if (m_base - m_lastUpdated > 1s)
			{
				std::cout << "FPS: " << m_ActualFPS << std::endl;
				m_lastUpdated = m_base;
				m_ActualFPS = 0;
				
			}

			if (m_lag > m_fixed_dt) {
				++m_ActualFPS;
				m_lag -= m_fixed_dt;
				return true;
			}

			return false;
		}

		std::pair<bool, float> FPSTimer::isDebugFPSShow() {
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

		float FPSTimer::getFPSCurrent() const { return m_ActualFPS; }
		float FPSTimer::getSecondsSinceStart() const {
			const std::chrono::duration<float, std::ratio<1>> t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_progStart);
			return t.count();
		}
	} // util
} //engn