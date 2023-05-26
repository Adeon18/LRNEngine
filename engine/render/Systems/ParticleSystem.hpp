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

			XMVECTOR m_particleColor;
			uint32_t m_positionMatrixIdx;
			uint32_t m_spawnRatePerSecond;
			uint32_t m_maxParticleCount;
			float m_spawnCircleRadius;

			std::shared_ptr<tex::Texture> m_particleAtlasDBF;

			std::vector<Particle> m_particles;

			static constexpr uint32_t PARTICLES_PER_FRAME = 1;
			static constexpr float PARTICLE_MIN_SIZE = 0.2f;
			static constexpr float PARTICLE_LIFETIME = 2.0f;
		};

		class ParticleSystem {
		public:
			enum class EMITTER_TYPES {
				SMOKE
			};

			struct PatricleTextureData {
				std::shared_ptr<tex::Texture> m_particleAtlasDBF;
				std::shared_ptr<tex::Texture> m_particleAtlasMVEA;
				std::shared_ptr<tex::Texture> m_particleAtlasRLU;
			};

			ParticleSystem(const ParticleSystem&) = delete;
			ParticleSystem& operator=(const ParticleSystem&) = delete;

			static ParticleSystem& getInstance() {
				static ParticleSystem p;
				return p;
			}

			template<typename ... Args>
			void addSmokeEmitter(Args&& ... args) {
				m_emitters[EMITTER_TYPES::SMOKE].emplace_back(std::forward<Args>(args)...);
			}

			//! Init all internal data
			void init();
			void handleParticles(std::unique_ptr<EngineCamera>& camPtr, float dt);
		private:
			void initBuffers();
			void initPipelines();
			void initTextures();

			//! Update the logic of all the particles
			void updateParticleLogic(std::unique_ptr<EngineCamera>& camPtr, float dt);
			//! Bind the particle relevant buffer data
			void bindBuffers(std::unique_ptr<EngineCamera>& camPtr);

			//! Bind patricleTextures for a specific emitterType
			void bindTextures(EMITTER_TYPES type);
			//! Fill the instance buffer for certain emitter type, should be called before render
			void fillInstanceBuffer(EMITTER_TYPES type);
			//! Render the particles of a certain emitter type
			void renderInternal(EMITTER_TYPES type);

			ParticleSystem() {}

			std::unordered_map<EMITTER_TYPES, std::vector<Emitter>> m_emitters;
			std::unordered_map<EMITTER_TYPES, PatricleTextureData> m_emitterTextures;

			InstanceBuffer<Particle> m_instanceBuffer;
			ConstantBuffer<CB_VS_ParticleData> m_particleData;

			Pipeline m_pipeline;
		};
	} // rend
} // engn