#pragma once


#include <array>
#include <vector>
#include <memory>

#include "source/math/ray.h"
#include "source/mesh/Mesh.h"

namespace engn {

	namespace mesh {
		class TriangleOctree
		{
		public:
			const static int PREFFERED_TRIANGLE_COUNT;
			const static float MAX_STRETCHING_RATIO;

			void clear() { m_mesh = nullptr; }
			bool inited() const { return m_mesh != nullptr; }

			//! Build the tree
			void initialize(const Mesh& mesh);
			// Check for intersection and fill the respective data
			bool intersect(const math::ray& ray, math::HitEntry& nearest) const;

		protected:
			const Mesh* m_mesh = nullptr;
			std::vector<uint32_t> m_triangles;

			BoundingBox m_box;
			BoundingBox m_initialBox;

			std::unique_ptr<std::array<TriangleOctree, 8>> m_children;
			//! Initialize bounding boxes
			void initialize(const Mesh& mesh, const BoundingBox& parentBox, const glm::vec3& parentCenter, int octetIndex);
			//! Figure out where to push a triangle
			bool addTriangle(uint32_t triangleIndex, const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3, const glm::vec3& center);
			//! Find the triangle that got intersected
			bool intersectInternal(const math::ray& ray, math::HitEntry& nearest) const;
		};

	} // mesh

} // engn