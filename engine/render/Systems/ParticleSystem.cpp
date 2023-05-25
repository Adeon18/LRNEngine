#include "ParticleSystem.hpp"

namespace engn {
	namespace rend {
		Emitter::Emitter(
			const XMVECTOR& pos,
			const XMVECTOR& col,
			const std::string& texturePath,
			uint32_t spawnRate,
			uint32_t maxCount,
			float spawnCircleRadius
		) : m_position{ pos }, m_particleColor{ col }, m_spawnRatePerSecond{ spawnRate }, m_maxParticleCount{ maxCount }, m_spawnCircleRadius{ spawnCircleRadius }
		{
			m_particleAtlas = tex::TextureManager::getInstance().getTexture(texturePath);
			m_particles.reserve(maxCount);
		}
	} // rend
} // engn