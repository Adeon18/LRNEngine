#pragma once

#include "render/Instances/Model.hpp"

#include "render/Graphics/VertexBuffer.hpp"
#include "render/Graphics/ConstantBuffer.hpp"

#include "render/Graphics/VertexShader.hpp"
#include "render/Graphics/PixelShader.hpp"

#include "render/Graphics/Vertex.hpp"

namespace engn {
	namespace rend {
		class NormalGroup {
		public:
			void addModel(std::shared_ptr<mdl::Model> mod);
			void fillInstanceBuffer();
			void render();
		private:
			struct Instance {
				XMMATRIX modelToWorld;
				XMFLOAT4 color;
			};

			struct Material {};

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
			VertexBuffer<Instance> m_instanceBuffer;
			ConstantBuffer<CB_VS_MeshData> m_meshData;
		};

		class MeshSystem {
		public:
			void render();
		private:
			NormalGroup m_normalGroup;
		};
	} // rend
} // engn