#pragma once

#include "Pipeline.hpp"

#include "render/Graphics/EngineCamera.hpp"

#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"
#include "render/Graphics/DXBuffers/InstanceBuffer.hpp"

#include "utils/TextureManager/TextureManager.hpp"

namespace engn {
	namespace rend {
		struct Particle {
			XMFLOAT4 colorAndAlpha;
			XMFLOAT3 centerPosition;
			XMFLOAT3 velocity;
			XMFLOAT2 size;
			float axisRotation;
			float lifeTime;
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
			void updateParticleData(std::unique_ptr<EngineCamera>& camPtr, float dt);
			//! Get the particle vector reference
			[[nodiscard]] std::vector<Particle>& getParticles();
			[[nodiscard]] const XMVECTOR& getPosition() const;
		private:
			//! Get the index of the first dead particle
			int getFirstDeadParticle();
			//! Fill the given particle with appropriate data
			void respawnParticle(Particle& particle, bool firstSpawn=false);

			XMVECTOR m_position;
			XMVECTOR m_particleColor;
			uint32_t m_spawnRatePerSecond;
			uint32_t m_maxParticleCount;
			float m_spawnCircleRadius;

			std::shared_ptr<tex::Texture> m_particleAtlas;

			std::vector<Particle> m_particles;

			static constexpr uint32_t PARTICLES_PER_FRAME = 1;
			static constexpr float PARTICLE_MIN_SIZE = 0.2f;
			static constexpr float PARTICLE_LIFETIME = 2.0f;
		};

		class ParticleSystem {
		public:
			ParticleSystem(const ParticleSystem&) = delete;
			ParticleSystem& operator=(const ParticleSystem&) = delete;

			static ParticleSystem& getInstance() {
				static ParticleSystem p;
				return p;
			}

			template<typename ... Args>
			void addSmokeEmitter(Args&& ... args) {
				m_emitters.emplace_back(std::forward<Args>(args)...);
			}
			//! Init all internal data
			void init();
			void handleParticles(std::unique_ptr<EngineCamera>& camPtr, float dt);
		private:
			void initBuffers();
			void initPipelines();

			//! Update the logic of all the particles
			void updateParticleLogic(std::unique_ptr<EngineCamera>& camPtr, float dt);
			//! Bind the particle relevant buffer data
			void bindBuffers(std::unique_ptr<EngineCamera>& camPtr);
			//! Fill the instance buffer, should be called before render
			void fillInstanceBuffer();
			//! Render the particles
			void renderInternal();

			ParticleSystem() {}

			std::vector<Emitter> m_emitters;

			InstanceBuffer<Particle> m_instanceBuffer;
			ConstantBuffer<CB_VS_ParticleData> m_particleData;

			Pipeline m_pipeline;
		};
	} // rend
} // engn