#include "Mesh.h"

namespace engn {

	namespace mesh {
		// Bounding Box
		glm::vec3 BoundingBox::size() const { return max - min; }
		glm::vec3 BoundingBox::center() const { return (min + max) / 2.f; }
		float BoundingBox::radius() const { return size().length() / 2.f; }

		void BoundingBox::expand(const BoundingBox& other) {
			min = other.min;
			max = other.max;
		}

		bool BoundingBox::contains(const glm::vec3& P) {
			return
				min[0] <= P[0] && P[0] <= max[0] &&
				min[1] <= P[1] && P[1] <= max[1] &&
				min[2] <= P[2] && P[2] <= max[2];
		}

		bool BoundingBox::hit(const math::ray& r, math::HitEntry& hitEntry) const {
			// x values
			float txMin = (min.x - r.origin.x) / r.direction.x;
			float txMax = (max.x - r.origin.x) / r.direction.x;

			float tMin = (std::min)(txMax, txMin);
			float tMax = (std::max)(txMax, txMin);

			// y values
			float tyMin = (min.y - r.origin.y) / r.direction.y;
			float tyMax = (max.y - r.origin.y) / r.direction.y;

			if (tyMin > tyMax) { std::swap(tyMin, tyMax); }

			if ((tMin > tyMax) || (tyMin > tMax)) { return false; }

			tMin = (std::max)(tMin, tyMin);
			tMax = (std::min)(tMax, tyMax);

			// z values
			float tzMin = (min.z - r.origin.z) / r.direction.z;
			float tzMax = (max.z - r.origin.z) / r.direction.z;

			if (tzMin > tzMax) { std::swap(tzMin, tzMax); }

			if ((tMin > tzMax) || (tzMin > tMax))
				return false;

			if (tzMin > tMin)
				tMin = tzMin;

			if (tzMax < tMax)
				tMax = tzMax;

			hitEntry.rayT = tMin;

			return true;
		}

		// Mesh
		Mesh::Mesh(const std::vector<math::triangle>& ts, const glm::vec3& min, const glm::vec3& max) : box{ min, max } { triangles = ts; }
		Mesh::Mesh(std::vector<math::triangle>&& ts, const glm::vec3& min, const glm::vec3& max) : box{ min, max } { triangles = std::move(ts); }
	} // mesh

} // engn
