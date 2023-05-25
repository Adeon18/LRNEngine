#pragma once

#include "Pipeline.hpp"

#include "utils/TextureManager/TextureManager.hpp"

namespace engn {
	namespace rend {
		struct Particle {
			XMFLOAT4 colorAndAlpha;
			XMFLOAT3 centerPosition;
			XMFLOAT3 velocity;
			XMFLOAT2 size;
			float axisRotation;
		};

		class Emitter {
		public:
			Emitter(
				const XMVECTOR& pos,
				const XMVECTOR& col,
				const std::string& texturePath,
				uint32_t spawnRate,
				uint32_t maxCount,
				float spawnCircleRadius
			);

			//! Spawn a few particles per frame
			void spawnParticles();
			//! Update the data of the particles
			void updateParticleData();
		private:
			XMVECTOR m_position;
			XMVECTOR m_particleColor;
			uint32_t m_spawnRatePerSecond;
			uint32_t m_maxParticleCount;
			float m_spawnCircleRadius;

			std::shared_ptr<tex::Texture> m_particleAtlas;

			std::vector<Particle> m_particles;
		};

		class ParticleSystem {
		public:
		private:
			std::vector<Emitter> m_emitters;
		};
	} // rend
} // engn