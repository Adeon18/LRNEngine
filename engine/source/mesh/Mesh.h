#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "source/math/ray.h"
#include "source/math/geometry/triangle.h"

namespace engn {

	namespace mesh {

		//! A bounding box class for faster mesh collision detection
		struct BoundingBox {
		public:
			static constexpr float Inf = std::numeric_limits<float>::infinity();
			static constexpr BoundingBox empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
			static constexpr BoundingBox unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

			//! Get size by diagonal
			glm::vec3 size() const { return max - min; }
			//! Get center point of box
			glm::vec3 center() const { return (min + max) / 2.f; }
			//! Get the radius of a outside sphere
			float radius() const { return size().length() / 2.f; }

			//! Change the BBs size
			void expand(const BoundingBox& other) {
				min = other.min;
				max = other.max;
			}
			//! Check if BB contains a point
			bool contains(const glm::vec3& P) {
				return
					min[0] <= P[0] && P[0] <= max[0] &&
					min[1] <= P[1] && P[1] <= max[1] &&
					min[2] <= P[2] && P[2] <= max[2];
			}

			bool hit(const math::ray& r, math::HitEntry& hitEntry) const {
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
		public:
			//! Member variables
			glm::vec3 min;
			glm::vec3 max;
		};

		//! A class which basically represents a vector of triangles
		struct Mesh {
			BoundingBox box;
			std::vector<math::triangle> triangles;

			Mesh() : box{} {};

			Mesh(const std::vector<math::triangle>& ts, const glm::vec3& min, const glm::vec3& max) : box{ min, max } {
				triangles = ts;
			}

			Mesh(std::vector<math::triangle>&& ts, const glm::vec3& min, const glm::vec3& max) : box{ min, max } {
				triangles = std::move(ts);
			}

			//! Manual iterative hit function for mesh - legacy, just for degug + it's slow + outdated
			math::HitEntry hit(math::ray& r) {
				math::HitEntry closest{};
				for (const auto& t: triangles) {
					if (t.hit(r, closest)) {}
				}
				return closest;
			}
		};

		//! Yes, I spend 30 minutes doing this
		static Mesh GET_BOX_MESH(const glm::vec3& min, const glm::vec3& max) {
			std::vector<math::triangle> cubeTriangles;
			cubeTriangles.reserve(12);

			// right side
			cubeTriangles.emplace_back(glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, min.y, max.z), glm::vec3(max.x, min.y, min.z));
			cubeTriangles.emplace_back(glm::vec3(max.x, max.y, min.z), glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, min.y, min.z));

			// left side
			cubeTriangles.emplace_back(glm::vec3(min.x, min.y, max.z), glm::vec3(min.x, max.y, max.z), glm::vec3(min.x, max.y, min.z));
			cubeTriangles.emplace_back(glm::vec3(min.x, min.y, min.z), glm::vec3(min.x, min.y, max.z), glm::vec3(min.x, max.y, min.z));	// bottom

			// front side
			cubeTriangles.emplace_back(glm::vec3(max.x, max.y, max.z), glm::vec3(min.x, max.y, max.z), glm::vec3(max.x, min.y, max.z));
			cubeTriangles.emplace_back(glm::vec3(min.x, min.y, max.z), glm::vec3(max.x, min.y, max.z), glm::vec3(min.x, max.y, max.z));

			// back side
			cubeTriangles.emplace_back(glm::vec3(max.x, max.y, min.z), glm::vec3(max.x, min.y, min.z), glm::vec3(min.x, max.y, min.z));	// bottom
			cubeTriangles.emplace_back(glm::vec3(min.x, max.y, min.z), glm::vec3(max.x, min.y, min.z), glm::vec3(min.x, min.y, min.z));	// top

			// top side
			cubeTriangles.emplace_back(glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, max.y, min.z), glm::vec3(min.x, max.y, min.z));
			cubeTriangles.emplace_back(glm::vec3(min.x, max.y, max.z), glm::vec3(max.x, max.y, max.z), glm::vec3(min.x, max.y, min.z));

			// bottom side
			cubeTriangles.emplace_back(glm::vec3(max.x, min.y, max.z), glm::vec3(min.x, min.y, max.z), glm::vec3(max.x, min.y, min.z));
			cubeTriangles.emplace_back(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, min.y, min.z), glm::vec3(min.x, min.y, max.z));

			return Mesh{ std::move(cubeTriangles), min, max };
		}

	} // mesh

} // engn