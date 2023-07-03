#include "ParticleSystem.hpp"

#include "MeshSystem.hpp"

#include "include/utility/utility.hpp"

namespace engn {
	namespace rend {
#ifdef _WIN64 
		const std::string SPHERE_MODEL_PATH = "..\\..\\assets\\Models\\Sphere\\sphere.fbx";
#else
		const std::string SPHERE_MODEL_PATH = "..\\assets\\Models\\Sphere\\sphere.fbx";
#endif // !_WIN64

		Emitter::Emitter(
			const XMVECTOR& pos,
			const XMVECTOR& col,
			uint32_t spawnRate,
			uint32_t maxCount,
			float spawnCircleRadius
		) : m_particleColor{ col }, m_spawnRatePerSecond{ spawnRate }, m_maxParticleCount{ maxCount }, m_spawnCircleRadius{ spawnCircleRadius }
		{
			m_particles.reserve(maxCount);
			m_positionMatrixIdx = MeshSystem::getInstance().addEmissionInstance(
				mdl::ModelManager::getInstance().getModel(util::getExeDir() + SPHERE_MODEL_PATH),
				{},
				{ XMMatrixScaling(m_spawnCircleRadius / 5.0f, m_spawnCircleRadius / 5.0f, m_spawnCircleRadius / 5.0f) * XMMatrixTranslationFromVector(pos), {}, {1.0f, 1.0f, 1.0f, 1.0f} }
			).first;
		}
		void Emitter::spawnParticles(float iTime)
		{
			if (m_particles.size() >= m_maxParticleCount) { return; }

			for (uint32_t i = 0; i < PARTICLES_PER_FRAME; ++i) {			
				auto& particle = m_particles.emplace_back();
				respawnParticle(particle, true, iTime);
			}
		}
		void Emitter::updateParticleData(std::unique_ptr<EngineCamera>& camPtr, float dt, float iTime)
		{
			int particlesPerFrame = m_spawnRatePerSecond;
			for (auto& particle : m_particles) {
				particle.lifeTime -= dt;
				if (particle.lifeTime > 0.0f)
				{	// particle is alive, thus update
					particle.centerPosition = {
						particle.centerPosition.x + particle.velocity.x * dt * 0.5f,
						particle.centerPosition.y + particle.velocity.y * dt * 0.5f,
						particle.centerPosition.z + particle.velocity.z * dt * 0.5f
					};
					particle.size.x += dt * 0.5f;
					particle.size.y += dt * 0.5f;
					if (particle.lifeTime > PARTICLE_LIFETIME / 2.0f) {
						particle.colorAndAlpha.w = (std::min)(1.0f, particle.colorAndAlpha.w + dt * PARTICLE_LIFETIME / 2.0f);
					}
					else {
						particle.colorAndAlpha.w = (std::max)(0.0f, particle.colorAndAlpha.w - dt * PARTICLE_LIFETIME / 2.0f);
					}
				} else {
					if (particlesPerFrame > 0) {
						respawnParticle(particle, false, iTime);
						--particlesPerFrame;
					}
				}
			}

			auto& camPos = camPtr->getCamPosition();
			// Sort after each update: TODO: THINK HOW TO OPTIMIZE
			std::sort(m_particles.begin(), m_particles.end(),
				[&camPos](const Particle& p1, const Particle& p2) {
					XMVECTOR p1Pos = XMLoadFloat3(&p1.centerPosition);
					XMVECTOR p2Pos = XMLoadFloat3(&p2.centerPosition);

					XMVECTOR p1DistLen = XMVector3Length(camPos - p1Pos);
					XMVECTOR p2DistLen = XMVector3Length(camPos - p2Pos);
					return (XMVectorGetX(p1DistLen) > XMVectorGetX(p2DistLen));
				}
			);
		}
		std::vector<Particle>& Emitter::getParticles()
		{
			return m_particles;
		}
		const XMVECTOR& Emitter::getPosition() const
		{
			// TODO: Kinda stupid
			XMMATRIX modelToWorld = TransformSystem::getInstance().getMatrixById(m_positionMatrixIdx);
			return XMVector3Transform({0.0f, 0.0f, 0.0f}, modelToWorld);
		}
		int Emitter::getFirstDeadParticle()
		{
			for (uint32_t i = 0; i < m_particles.size(); ++i) {
				if (m_particles[i].lifeTime < 0.0f) { return i; }
			}
			return -1;
		}

		void Emitter::respawnParticle(Particle& particle, bool firstSpawn, float iTime)
		{
			auto pos = getPosition();
			XMFLOAT3 particlePos{
				XMVectorGetX(pos) + static_cast<float>(util::getRandomIntInRange(-100, 100) % 100) / 100.0f * m_spawnCircleRadius,
				XMVectorGetY(pos) + static_cast<float>(util::getRandomIntInRange(-100, 100) % 100) / 100.0f * m_spawnCircleRadius,
				XMVectorGetZ(pos) + static_cast<float>(util::getRandomIntInRange(-100, 100) % 100) / 100.0f * m_spawnCircleRadius,
			};
			particle.centerPosition = particlePos;
			particle.colorAndAlpha = { XMVectorGetX(m_particleColor), XMVectorGetY(m_particleColor), XMVectorGetZ(m_particleColor), 0.0f };
			particle.velocity = {
				0.0f,
				static_cast<float>(util::getRandomIntInRange(5, 10)) / 10.0f,
				0.0f,
			};
			particle.size = { PARTICLE_MIN_SIZE, PARTICLE_MIN_SIZE };
			particle.axisRotation = XMConvertToRadians(static_cast<float>(rand() % 3600) / 10.0f);
			particle.spawnAtTime = iTime;
			//! TODO: Since we have only one particle type for not it is ok, but, this needs to be computed differently later
			float particleLifetimeSmoke = 64.0f / UI::instance().getParticleWidgetData().animationFPS;
			particle.lifeTime = particleLifetimeSmoke;
		}

		void ParticleSystem::init()
		{
			initPipelines();
			initBuffers();
			initTextures();
			initShaders();
		}
		void ParticleSystem::handleParticles(std::unique_ptr<EngineCamera>& camPtr, float dt, float iTime)
		{
			updateParticleLogic(camPtr, dt, iTime);

			bindPipeline(m_pipelineCPU);

			bindBuffers(camPtr, EMITTER_TYPES::SMOKE);
			bindTextures(EMITTER_TYPES::SMOKE);
			fillInstanceBuffer(EMITTER_TYPES::SMOKE);
			renderInternal(EMITTER_TYPES::SMOKE);
		}
		void ParticleSystem::handleGPUParticles(std::unique_ptr<EngineCamera>& camPtr, float dt, float iTime)
		{

			m_ringBuffer.bindToCS();
			m_particlePhysicsCS.bind();

			d3d::s_devcon->Dispatch(2048, 1, 1);

			m_indirectDrawCS.bind();
			d3d::s_devcon->Dispatch(1, 1, 1);

			m_ringBuffer.bindToPipeline();
			m_particleGPUIB.bind();
			bindTexturesGPU();
			bindBuffers(camPtr, EMITTER_TYPES::SMOKE);
			bindPipeline(m_pipelineGPULighting);
			d3d::s_devcon->DrawIndexedInstancedIndirect(m_ringBuffer.getIndirectBufferPtr(), 12);

			bindPipeline(m_pipelineGPU);
			d3d::s_devcon->DrawIndexedInstancedIndirect(m_ringBuffer.getIndirectBufferPtr(), 12);
		}
		void ParticleSystem::bindUAVs()
		{
			m_ringBuffer.bindToPipeline();
		}
		void ParticleSystem::initBuffers()
		{
			m_particleDataVS.init();
			m_particleDataPS.init();
			m_ringBuffer.init();

			std::vector<DWORD> indices{
				0, 1, 2, 3, 2, 1
			};

			m_particleGPUIB.init(indices);
		}
		void ParticleSystem::initPipelines()
		{
			D3D11_INPUT_ELEMENT_DESC DEFAULT_LAYOUT_PARTICLES[5] = {
				{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"SIZE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 1, 28, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"ROTATION", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, 36, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
				{"SPAWNTIME", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, 40, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1}
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

			PipelineData pipelineDataCPU{
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

			initPipeline(m_pipelineCPU, pipelineDataCPU);

			PipelineData pipelineDataGPU{
				nullptr,
				0,
				D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				shaderFolder + L"VSGPUParticles.cso",
				L"",
				L"",
				L"",
				shaderFolder + L"PSGPUParticles.cso",
				rasterizerDesc,
				depthStencilDesc,
				blendDesc
			};

			initPipeline(m_pipelineGPU, pipelineDataGPU);

			D3D11_RENDER_TARGET_BLEND_DESC blendDescAdditive{};
			blendDescAdditive.BlendEnable = true;
			blendDescAdditive.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			blendDescAdditive.DestBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			blendDescAdditive.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blendDescAdditive.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			blendDescAdditive.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			blendDescAdditive.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blendDescAdditive.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			PipelineData pipelineDataGPULighting{
				nullptr,
				0,
				D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				shaderFolder + L"VSGPUParticleLighting.cso",
				L"",
				L"",
				L"",
				shaderFolder + L"PSGPUParticleLighting.cso",
				rasterizerDesc,
				depthStencilDesc,
				blendDescAdditive
			};

			initPipeline(m_pipelineGPULighting, pipelineDataGPULighting);
		}
		void ParticleSystem::initTextures()
		{
#ifdef _WIN64 
			const std::string SMOKE_DBF = "..\\..\\assets\\Textures\\Atlases\\Particles\\smoke_DBF.dds";
			const std::string SMOKE_MVEA = "..\\..\\assets\\Textures\\Atlases\\Particles\\smoke_MVEA.dds";
			const std::string SMOKE_RLU = "..\\..\\assets\\Textures\\Atlases\\Particles\\smoke_RLU.dds";
			const std::string SPARK_TEX = "..\\..\\assets\\Textures\\Particles\\explosion.dds";
#else
			const std::string SMOKE_DBF = "..\\assets\\Textures\\Atlases\\Particles\\smoke_DBF.dds";
			const std::string SMOKE_MVEA = "..\\assets\\Textures\\Atlases\\Particles\\smoke_MVEA.dds";
			const std::string SMOKE_RLU = "..\\assets\\Textures\\Atlases\\Particles\\smoke_RLU.dds";
			const std::string SPARK_TEX = "..\\assets\\Textures\\Particles\\explosion.dds";
#endif // !_WIN64

			std::string exeDir = util::getExeDir();
			auto& smoke = m_emitterTextures[EMITTER_TYPES::SMOKE];
			smoke.m_particleAtlasDBF = tex::TextureManager::getInstance().getTexture(exeDir + SMOKE_DBF);
			smoke.m_particleAtlasMVEA = tex::TextureManager::getInstance().getTexture(exeDir + SMOKE_MVEA);
			smoke.m_particleAtlasRLU = tex::TextureManager::getInstance().getTexture(exeDir + SMOKE_RLU);
			smoke.frameCountH = 8;
			smoke.frameCountV = 8;

			m_sparkTexture = tex::TextureManager::getInstance().getTexture(exeDir + SPARK_TEX);
		}
		void ParticleSystem::initShaders()
		{
			m_particlePhysicsCS.init(SHADER_FOLDER + L"CSParticlePhysics.cso");
			m_indirectDrawCS.init(SHADER_FOLDER + L"CSParticleIndirectDrawUpdate.cso");
		}
		void ParticleSystem::updateParticleLogic(std::unique_ptr<EngineCamera>& camPtr, float dt, float iTime)
		{
			// TODO WARNING: WORKS ONLY WOR SMOKE
			for (auto& emitter : m_emitters[EMITTER_TYPES::SMOKE]) {
				emitter.spawnParticles(iTime);
				emitter.updateParticleData(camPtr, dt, iTime);
			}

			auto& camPos = camPtr->getCamPosition();
			auto& camFwd = camPtr->getCamForward();

			std::sort(m_emitters[EMITTER_TYPES::SMOKE].begin(), m_emitters[EMITTER_TYPES::SMOKE].end(),
				[&camPos, &camFwd](const Emitter& e1, const Emitter& e2) {

				XMVECTOR p1Dist = e1.getPosition() - camPos;
				XMVECTOR p2Dist = e2.getPosition() - camPos;
				return (XMVectorGetX(XMVector3Dot(p1Dist, camFwd)) > XMVectorGetX(XMVector3Dot(p2Dist, camFwd)));
				}
			);
		}
		void ParticleSystem::bindBuffers(std::unique_ptr<EngineCamera>& camPtr, EMITTER_TYPES type)
		{
			auto& pData = m_particleDataVS.getData();
			pData.cameraPosition = camPtr->getCamPosition();
			m_particleDataVS.fill();
			d3d::s_devcon->VSSetConstantBuffers(1, 1, m_particleDataVS.getBufferAddress());


			auto& particleWidget = UI::instance().getParticleWidgetData();
			auto& pDataPS = m_particleDataPS.getData();
			auto& pTexData = m_emitterTextures[type];
			pDataPS.atlasFrameCount = { pTexData.frameCountH, pTexData.frameCountH, pTexData.frameCountV, pTexData.frameCountV };
			pDataPS.animationSpeedFPS = { particleWidget.animationFPS, particleWidget.animationFPS, particleWidget.animationFPS, particleWidget.animationFPS };
			m_particleDataPS.fill();
			d3d::s_devcon->PSSetConstantBuffers(1, 1, m_particleDataPS.getBufferAddress());
		}
		void ParticleSystem::bindTextures(EMITTER_TYPES type)
		{
			d3d::s_devcon->PSSetShaderResources(0, 1, m_emitterTextures[type].m_particleAtlasDBF->textureView.GetAddressOf());
			d3d::s_devcon->PSSetShaderResources(1, 1, m_emitterTextures[type].m_particleAtlasMVEA->textureView.GetAddressOf());
			d3d::s_devcon->PSSetShaderResources(2, 1, m_emitterTextures[type].m_particleAtlasRLU->textureView.GetAddressOf());
		}
		void ParticleSystem::bindTexturesGPU()
		{
			d3d::s_devcon->PSSetShaderResources(0, 1, m_sparkTexture->textureView.GetAddressOf());
		}
		void ParticleSystem::fillInstanceBuffer(EMITTER_TYPES type)
		{
			// Count total instances
			uint32_t totalInstances = 0;
			for (auto& emitterType : m_emitters) {
				for (auto& emitter : emitterType.second) {
					totalInstances += uint32_t(emitter.getParticles().size());
				}
			}

			if (totalInstances == 0)
				return;

			// Initialize instanceBuffer
			m_instanceBuffer.init(totalInstances); // resizes if needed

			// Map buffer data
			if (!m_instanceBuffer.map()) {
				return;
			}
			ParticleInstance* dst = static_cast<ParticleInstance*>(m_instanceBuffer.getMappedBuffer().pData);

			// Fill mapped buffer
			uint32_t copiedNum = 0;
			for (auto& emitter: m_emitters[type])
			{
				for (auto& particle: emitter.getParticles())
				{
					ParticleInstance p;
					p.colorAndAlpha = particle.colorAndAlpha;
					p.centerPosition = particle.centerPosition;
					p.axisRotation = particle.axisRotation;
					p.size = particle.size;
					p.spawnTime = particle.spawnAtTime;
					dst[copiedNum++] = p;
				}
			}

			m_instanceBuffer.unmap();
		}
		void ParticleSystem::renderInternal(EMITTER_TYPES type)
		{
			// TODO: DON'T BIND SJADERS IF EMPTY
			if (m_instanceBuffer.getSize() == 0)
				return;

			m_instanceBuffer.bind();

			uint32_t renderedInstances = 0;
			for (auto& emitter : m_emitters[type])
			{
				auto& particles = emitter.getParticles();
				uint32_t numInstances = particles.size();
				d3d::s_devcon->DrawInstanced(4, numInstances, 0, renderedInstances);
				renderedInstances += numInstances;
			}
		}
	} // rend
} // engn