#pragma once

#include "utils/ModelManager/ModelManager.hpp"
#include "render/Instances/Model.hpp"

#include "render/Graphics/VertexBuffer.hpp"
#include "render/Graphics/ConstantBuffer.hpp"
#include "render/Graphics/InstanceBuffer.hpp"

#include "render/Graphics/VertexShader.hpp"
#include "render/Graphics/PixelShader.hpp"

#include "render/Graphics/Vertex.hpp"

namespace engn {
	namespace rend {
		class NormalGroup {
		public:
			struct Instance {
				XMMATRIX modelToWorld;
				XMFLOAT4 color;
			};

			struct Material {
				bool operator==(const Material& other) {
					return true;
				}
			};

			struct PerMaterial {
				Material material;
				std::vector<Instance> instances;
			};

			using PerMesh = std::vector<PerMaterial>;

			struct PerModel {
				std::shared_ptr<mdl::Model> model;
				std::vector<PerMesh> perMesh;
			};

			std::vector<PerModel> m_models;
			InstanceBuffer<Instance> m_instanceBuffer;
			ConstantBuffer<CB_VS_MeshData> m_meshData;

			VertexShader m_vertexShader;
			PixelShader m_pixelShader;
		public:
			void init();
			void addModel(std::shared_ptr<mdl::Model> mod, const Material& mtrl, const Instance& inc);
			//! Fill the data to be passed by instance
			void fillInstanceBuffer(const XMMATRIX& worldToView);
			void render();
		};

		class MeshSystem {
		public:
			static MeshSystem& getInstance() {
				static MeshSystem system;
				return system;
			}
			MeshSystem(const MeshSystem& other) = delete;
			MeshSystem& operator=(const MeshSystem& other) = delete;

			void init() {
				m_normalGroup.init();
			}

			void render(const XMMATRIX& worldToClip);
			
			void addNormalInstance(std::shared_ptr<mdl::Model> mod, const NormalGroup::Material& mtrl, const NormalGroup::Instance& inc);
		private:
			MeshSystem() {};
			NormalGroup m_normalGroup;
		};
	} // rend
} // engn