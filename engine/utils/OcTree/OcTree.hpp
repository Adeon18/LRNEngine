#pragma once

#include <memory>
#include <array>

#include "source/mesh/Mesh.hpp"
#include "source/math/Ray.hpp"

namespace engn {
	namespace util {
		using namespace DirectX;
		class TriangleOctree
		{
		public:
			const static int PREFFERED_TRIANGLE_COUNT;
			const static float MAX_STRETCHING_RATIO;

			void clear() { m_mesh = nullptr; }
			bool inited() const { return m_mesh != nullptr; }

			void initialize(const mdl::Mesh& mesh);

			bool intersect(const geom::Ray& ray, mdl::MeshIntersection& nearest) const;

			[[nodiscard]] const mdl::BoundingBox& getBox() const { return m_box; }

		protected:
			const mdl::Mesh* m_mesh = nullptr;
			std::vector<uint32_t> m_triangles;

			mdl::BoundingBox m_box;
			mdl::BoundingBox m_initialBox;

			std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

			void initialize(const mdl::Mesh& mesh, const mdl::BoundingBox& parentBox, const XMVECTOR& parentCenter, int octetIndex);

			bool addTriangle(uint32_t triangleIndex, const XMVECTOR& V1, const XMVECTOR& V2, const XMVECTOR& V3, const XMVECTOR& center);

			bool intersectInternal(const geom::Ray& ray, mdl::MeshIntersection& nearest) const;
		};
	} // util
} // engn