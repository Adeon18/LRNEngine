#include "ParticleSystem.hpp"

#include "include/utility/utility.hpp"

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
		void Emitter::spawnParticles()
		{
			for (uint32_t i = 0; i < PARTICLES_PER_FRAME; ++i) {
				if (m_particles.size() >= m_maxParticleCount) { return; }
			
				auto& particle = m_particles.emplace_back();
				respawnParticle(particle);
			}
		}
		void Emitter::updateParticleData(float dt)
		{
			for (auto& particle : m_particles) {
				particle.lifeTime -= dt;
				if (particle.lifeTime > 0.0f)
				{	// particle is alive, thus update
					particle.centerPosition = {
						particle.centerPosition.x + particle.velocity.x * dt,
						particle.centerPosition.y + particle.velocity.y * dt,
						particle.centerPosition.z + particle.velocity.z * dt
					};
					particle.size.x += dt * 0.1f;
					particle.size.y += dt * 0.1f;
					if (particle.lifeTime > PARTICLE_LIFETIME / 2.0f) {
						particle.colorAndAlpha.w = (std::min)(1.0f, particle.colorAndAlpha.w + dt * 2.5f);
					}
					else {
						particle.colorAndAlpha.w = (std::max)(0.0f, particle.colorAndAlpha.w - dt * PARTICLE_LIFETIME / 2.0f);
					}
				} else {
					respawnParticle(particle);
				}
			}
		}
		std::vector<Particle>& Emitter::getParticles()
		{
			return m_particles;
		}
		int Emitter::getFirstDeadParticle()
		{
			for (uint32_t i = 0; i < m_particles.size(); ++i) {
				if (m_particles[i].lifeTime < 0.0f) { return i; }
			}
			return -1;
		}

		void Emitter::respawnParticle(Particle& particle)
		{
			XMFLOAT3 particlePos{
					XMVectorGetX(m_position) + static_cast<float>(util::getRandomIntInRange(-100, 100) % 100) / 100.0f * m_spawnCircleRadius,
					XMVectorGetY(m_position) + static_cast<float>(util::getRandomIntInRange(-100, 100) % 100) / 100.0f * m_spawnCircleRadius,
					XMVectorGetZ(m_position) + static_cast<float>(util::getRandomIntInRange(-100, 100) % 100) / 100.0f * m_spawnCircleRadius,
			};
			particle.colorAndAlpha = { XMVectorGetX(m_particleColor), XMVectorGetY(m_particleColor), XMVectorGetZ(m_particleColor), 0.0f };
			particle.centerPosition = particlePos;
			particle.velocity = { 0.0f, 1.0f, 0.0f };
			particle.size = { PARTICLE_MIN_SIZE, PARTICLE_MIN_SIZE };
			particle.axisRotation = static_cast<float>(rand() % 3600) / 10.0f;
			particle.lifeTime = PARTICLE_LIFETIME;
		}

		void ParticleSystem::init()
		{
			initPipelines();
			initBuffers();
		}
		void ParticleSystem::handleParticles(std::unique_ptr<EngineCamera>& camPtr, float dt)
		{
			updateParticleLogic(dt);

			bindBuffers(camPtr);

			bindPipeline(m_pipeline);

			fillInstanceBuffer();

			renderInternal();
		}
		void ParticleSystem::initBuffers()
		{
			m_particleData.init();
		}
		void ParticleSystem::initPipelines()
		{
			D3D11_INPUT_ELEMENT_DESC DEFAULT_LAYOUT_PARTICLES[6] = {
				{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"VELOCITY", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, 28, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"SIZE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 1, 40, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"ROTATION", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"LIFE", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, 52, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1}
			};

			auto shaderFolder = util::getExeDirW();


			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;

			D3D11_RENDER_TARGET_BLEND_DESC blendDesc{};
			blendDesc.BlendEnable = true;
			blendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			blendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			blendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
			blendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			PipelineData pipelineData{
				DEFAULT_LAYOUT_PARTICLES,
				ARRAYSIZE(DEFAULT_LAYOUT_PARTICLES),
				D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
				shaderFolder + L"VSParticle.cso",
				L"",
				L"",
				L"",
				shaderFolder + L"PSParticle.cso",
				rasterizerDesc,
				depthStencilDesc,
				blendDesc
			};

			initPipeline(m_pipeline, pipelineData);
		}
		void ParticleSystem::updateParticleLogic(float dt)
		{
			for (auto& emitter : m_emitters) {
				emitter.spawnParticles();
				emitter.updateParticleData(dt);
			}
		}
		void ParticleSystem::bindBuffers(std::unique_ptr<EngineCamera>& camPtr)
		{
			auto& pData = m_particleData.getData();
			pData.cameraPosition = camPtr->getCamPosition();
			m_particleData.fill();
			d3d::s_devcon->VSSetConstantBuffers(1, 1, m_particleData.getBufferAddress());
		}
		void ParticleSystem::fillInstanceBuffer()
		{
			// Count total instances
			uint32_t totalInstances = 0;
			for (auto& emitter : m_emitters) {
				totalInstances += uint32_t(emitter.getParticles().size());
			}

			if (totalInstances == 0)
				return;

			// Initialize instanceBuffer
			m_instanceBuffer.init(totalInstances); // resizes if needed

			// Map buffer data
			if (!m_instanceBuffer.map()) {
				return;
			}
			Particle* dst = static_cast<Particle*>(m_instanceBuffer.getMappedBuffer().pData);

			// Fill mapped buffer
			uint32_t copiedNum = 0;
			for (auto& emitter: m_emitters)
			{
				for (auto& particle: emitter.getParticles())
				{
					dst[copiedNum++] = particle;
				}
			}

			m_instanceBuffer.unmap();
		}
		void ParticleSystem::renderInternal()
		{
			// TODO: DON'T BIND SJADERS IF EMPTY
			if (m_instanceBuffer.getSize() == 0)
				return;

			m_instanceBuffer.bind();

			uint32_t renderedInstances = 0;
			for (auto& emitter : m_emitters)
			{
				auto& particles = emitter.getParticles();
				uint32_t numInstances = particles.size();
				d3d::s_devcon->DrawInstanced(4, numInstances, 0, renderedInstances);
				renderedInstances += numInstances;
			}
		}
	} // rend
} // engn