#include "Model.hpp"

namespace engn {
	namespace mdl {
		void Model::fillBuffersFromMeshes() {
			uint32_t totalVerticeCount = 0;
			uint32_t totalIndiceCount = 0;
			for (auto& mesh : m_meshes) {
				totalVerticeCount += mesh.vertices.size();
				totalIndiceCount += mesh.triangles.size() * 3;
			}

			std::vector<Vertex> vertices(totalVerticeCount);
			std::vector<DWORD> indices(totalIndiceCount);

			uint32_t totalVertexOffset = 0;
			uint32_t totalIndexOffset = 0;
			for (auto& mesh : m_meshes) {
				m_ranges.push_back(MeshRange{ totalVertexOffset, totalIndexOffset, static_cast<uint32_t>(mesh.vertices.size()), static_cast<uint32_t>(mesh.triangles.size() * 3) });
				for (uint32_t i = 0; i < mesh.vertices.size(); ++i) {
					vertices[totalVertexOffset + i] = mesh.vertices[i];
				}
				for (uint32_t i = 0; i < mesh.triangles.size(); ++i) {
					indices[totalIndexOffset + i * 3] = mesh.triangles[i].indices[0];
					indices[totalIndexOffset + i * 3 + 1] = mesh.triangles[i].indices[1];
					indices[totalIndexOffset + i * 3 + 2] = mesh.triangles[i].indices[2];
				}
				totalVertexOffset += mesh.vertices.size();
				totalIndexOffset += mesh.triangles.size() * 3;
			}

			m_vertices.init(vertices);
			m_indices.init(indices);
		}
	} // mdl
} // engn