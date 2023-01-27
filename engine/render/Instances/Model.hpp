#pragma once

#include "source/mesh/Mesh.hpp"

#include "render/Graphics/Vertex.hpp"
#include "render/Graphics/VertexBuffer.hpp"
#include "render/Graphics/IndexBuffer.hpp"

namespace engn {
	namespace model {
		class Model {
		public:
			struct MeshRange
			{
				uint32_t vertexOffset; // offset in vertices
				uint32_t indexOffset; // offset in indices
				uint32_t vertexNum; // num of vertices
				uint32_t indexNum; // num of indices
			};

			std::vector<Mesh>& getMeshes() { return m_meshes; }
			std::vector<MeshRange>& getRanges() { return m_ranges; }
			rend::VertexBuffer<Vertex>& getVertices() { return m_vertices; }
			rend::IndexBuffer& getIndices() { return m_indices; }
		protected:
			std::vector<Mesh> m_meshes;  // TO CHANGE!!!!!!!!!!!!!
			std::vector<MeshRange> m_ranges; // where each mesh data is stored in m_vertices
			rend::VertexBuffer<Vertex> m_vertices; // stores vertices of all meshes of this Model
			rend::IndexBuffer m_indices; // stores vertex indices of all meshes of this Model
		};
	} // model
} // engn