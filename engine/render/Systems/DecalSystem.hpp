#pragma once

#include <array>

#include "Pipeline.hpp"

#include "render/Graphics/EngineCamera.hpp"

#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"
#include "render/Graphics/DXBuffers/InstanceBuffer.hpp"

#include "render/Systems/MeshSystem.hpp"

namespace engn {
	namespace rend {
		class DecalSystem {
		public:
			struct DecalData {
				XMMATRIX decalToModel;
				XMMATRIX modelToDecal;
				XMVECTOR color;
				uint32_t modelInstanceID;
				uint32_t objectID;
			};

			struct DecalInstance {
				XMMATRIX decalToWorld;
				XMMATRIX worldToDecal;
				XMVECTOR color;
				uint32_t objectID;
			};

			static DecalSystem& getInstance() {
				static DecalSystem d;
				return d;
			}
			DecalSystem(const DecalSystem& d) = delete;
			DecalSystem& operator=(const DecalSystem& d) = delete;

			//! Init the entire system
			void init();
			
			//! Initialize new DecalData(build basis)
			void addDecal(const InstanceProperties& insProps, const geom::MeshIntersection& insHit, std::unique_ptr<EngineCamera>& camPtr);

			//! Handle the Decal Render(render and filling instance buffers)
			void handleDecals();

			//! Removes decals bound to a certain instance by the transform matrix instance id(needs to be done at deletion of instance)
			void clearDecalsByInstanceIdx(uint32_t insIdx);
		private:
			DecalSystem() {}

			void initBuffers();
			void initPipelines();
			void initTextures();
			void initModels();

			void fillInstanceBuffers();

			void renderInternal();

			std::vector<DecalData> m_decals;

			InstanceBuffer<DecalInstance> m_instanceBuffer;

			Pipeline m_pipeline;

			std::shared_ptr<mdl::Model> m_cubeModel;

			std::shared_ptr<tex::Texture> m_splatterNormalMap;
			
			static constexpr std::array<XMVECTOR, 10> DECAL_COLORS{
				XMVECTOR{1.0f, 0.0f, 0.0f, 1.0f},
				XMVECTOR{0.0f, 1.0f, 0.0f, 1.0f},
				XMVECTOR{1.0f, 1.0f, 0.0f, 1.0f},
				XMVECTOR{0.0f, 0.0f, 1.0f, 1.0f},
				XMVECTOR{1.0f, 0.0f, 1.0f, 1.0f},
				XMVECTOR{0.0f, 1.0f, 1.0f, 1.0f},
				XMVECTOR{1.0f, 1.0f, 1.0f, 1.0f},
				XMVECTOR{1.0f, 0.5f, 0.5f, 1.0f},
				XMVECTOR{1.0f, 0.0f, 0.5f, 1.0f},
				XMVECTOR{1.0f, 0.5f, 0.0f, 1.0f}
			};
		};
	} // rend
} // engn