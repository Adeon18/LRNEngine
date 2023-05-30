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
			float spawnAtTime;
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
			void spawnParticles(float iTime);
			//! Update the data of the particles
			void updateParticleData(std::unique_ptr<EngineCamera>& camPtr, float dt, float iTime);
			//! Get the particle vector reference
			[[nodiscard]] std::vector<Particle>& getParticles();
			[[nodiscard]] const XMVECTOR& getPosition() const;
		private:
			//! Get the index of the first dead particle
			int getFirstDeadParticle();
			//! Fill the given particle with appropriate data
			void respawnParticle(Particle& particle, bool firstSpawn, float iTime);

			XMVECTOR m_particleColor;
			uint32_t m_positionMatrixIdx;
			uint32_t m_spawnRatePerSecond;
			uint32_t m_maxParticleCount;
			float m_spawnCircleRadius;

			std::shared_ptr<tex::Texture> m_particleAtlasDBF;

			std::vector<Particle> m_particles;

			static constexpr uint32_t PARTICLES_PER_FRAME = 1;
			static constexpr float PARTICLE_MIN_SIZE = 0.4f;
			static constexpr float PARTICLE_LIFETIME = 4.0f;
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
				int32_t frameCountH;
				int32_t frameCountV;
			};

			struct ParticleInstance {
				XMFLOAT4 colorAndAlpha;
				XMFLOAT3 centerPosition;
				XMFLOAT2 size;
				float axisRotation;
				float spawnTime;
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
			void handleParticles(std::unique_ptr<EngineCamera>& camPtr, float dt, float iTime);

			//! These getters are here to get filled in rendered by the respective Window class fucntion(which copies the current depth texture)
			[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Texture2D>& getDepthTextureRef() { return m_currentDepthTexture; }
			[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getDepthTextureSRVRef() { return m_currentDepthSRV; }
		private:
			void initBuffers();
			void initPipelines();
			void initTextures();

			//! Update the logic of all the particles
			void updateParticleLogic(std::unique_ptr<EngineCamera>& camPtr, float dt, float iTime);
			//! Bind the particle relevant buffer data
			void bindBuffers(std::unique_ptr<EngineCamera>& camPtr, EMITTER_TYPES type);

			//! Bind patricleTextures for a specific emitterType
			void bindTextures(EMITTER_TYPES type);
			//! Fill the instance buffer for certain emitter type, should be called before render
			void fillInstanceBuffer(EMITTER_TYPES type);
			//! Render the particles of a certain emitter type
			void renderInternal(EMITTER_TYPES type);

			ParticleSystem() {}

			std::unordered_map<EMITTER_TYPES, std::vector<Emitter>> m_emitters;
			std::unordered_map<EMITTER_TYPES, PatricleTextureData> m_emitterTextures;

			InstanceBuffer<ParticleInstance> m_instanceBuffer;
			ConstantBuffer<CB_VS_ParticleData> m_particleDataVS;
			ConstantBuffer<CB_PS_ParticleData> m_particleDataPS;

			Pipeline m_pipeline;

			//! Current depth textures before the particle render to avoid smooth clipping
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_currentDepthTexture;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_currentDepthSRV;
		};
	} // rend
} // engn