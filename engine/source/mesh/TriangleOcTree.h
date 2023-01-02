#pragma once


#include <vector>
#include <array>
#include <memory>

#include "source/mesh/Mesh.h"
#include "source/math/ray.h"

namespace engn {

namespace mesh {

	struct MeshIntersection
	{
		glm::vec3 pos;
		glm::vec3 normal;
		float nr;
		float t;
		uint32_t triangle;

		bool valid() const { return std::isfinite(t); }
	};

	class TriangleOctree
	{
	public:
		const static int PREFFERED_TRIANGLE_COUNT;
		const static float MAX_STRETCHING_RATIO;

		void clear() { m_mesh = nullptr; }
		bool inited() const { return m_mesh != nullptr; }

		void initialize(const Mesh& mesh);

		bool intersect(const math::ray& ray, math::HitEntry& nearest) const;

	protected:
		const Mesh* m_mesh = nullptr;
		std::vector<uint32_t> m_triangles;

		BoundingBox m_box;
		BoundingBox m_initialBox;

		std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

		void initialize(const Mesh& mesh, const BoundingBox& parentBox, const glm::vec3& parentCenter, int octetIndex);

		bool addTriangle(uint32_t triangleIndex, const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3, const glm::vec3& center);

		bool intersectInternal(const math::ray& ray, math::HitEntry& nearest) const;
	};

} // mesh

} // engn