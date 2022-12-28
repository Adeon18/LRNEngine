#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "source/math/ray.h"
#include "source/math/geometry/triangle.h"

namespace engn {

namespace mesh {

	//! A class which basically represents a vector of triangles
	struct Mesh {
	public:
		Mesh() {};

		Mesh(const std::vector<math::triangle>& ts) {
			triangles = ts;
		}

		Mesh(std::vector<math::triangle>&& ts) {
			triangles = std::move(ts);
		}

		math::HitEntry hit(const math::ray& r) {
			int closestTriangleIdx = 0;
			math::HitEntry closestTriangle = triangles[0].hit(r);
			for (size_t i = 1; i < triangles.size(); ++i) {
				auto collisionLog = triangles[i].hit(r);
				if (collisionLog.isHit && collisionLog.rayT > 0 && collisionLog.rayT < closestTriangle.rayT) {
					closestTriangle = collisionLog;
					closestTriangleIdx = i;
				}
			}
			return closestTriangle;
		}

		std::vector<math::triangle> triangles;
	};

	struct BoundingBox {
		static constexpr float Inf = std::numeric_limits<float>::infinity();
		static constexpr BoundingBox empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
		static constexpr BoundingBox unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

		glm::vec3 size() const { return max - min; }
		glm::vec3 center() const { return (min + max) / 2.f; }
		float radius() const { return size().length() / 2.f; }

		void expand(const BoundingBox& other)
		{
			min = other.min;
			max = other.max;
		}

		bool contains(const glm::vec3& P)
		{
			return
				min[0] <= P[0] && P[0] <= max[0] &&
				min[1] <= P[1] && P[1] <= max[1] &&
				min[2] <= P[2] && P[2] <= max[2];
		}

		//! Member variables
		glm::vec3 min;
		glm::vec3 max;
	};

	static Mesh GET_BOX_MESH(const glm::vec3& min, const glm::vec3& max) {
		std::vector<math::triangle> cubeTriangles;
		cubeTriangles.reserve(12);

		// right side
		cubeTriangles.emplace_back(glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, min.y, max.z), glm::vec3(max.x, min.y, min.z));
		cubeTriangles.emplace_back(glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, max.y, min.z), glm::vec3(max.x, min.y, min.z));

		// left side
		cubeTriangles.emplace_back(glm::vec3(min.x, max.y, max.z), glm::vec3(min.x, min.y, max.z), glm::vec3(min.x, max.y, min.z));
		cubeTriangles.emplace_back(glm::vec3(min.x, min.y, max.z), glm::vec3(min.x, min.y, min.z), glm::vec3(min.x, max.y, min.z));

		// front side
		cubeTriangles.emplace_back(glm::vec3(max.x, max.y, max.z), glm::vec3(max.x, min.y, max.z), glm::vec3(min.x, max.y, max.z));
		cubeTriangles.emplace_back(glm::vec3(min.x, min.y, max.z), glm::vec3(max.x, min.y, max.z), glm::vec3(min.x, max.y, max.z));

		// back side
		cubeTriangles.emplace_back(glm::vec3(max.x, max.y, min.z), glm::vec3(max.x, min.y, min.z), glm::vec3(min.x, max.y, min.z));
		cubeTriangles.emplace_back(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, min.y, min.z), glm::vec3(min.x, max.y, min.z));

		// top side
		cubeTriangles.emplace_back(glm::vec3(max.x, max.y, min.z), glm::vec3(max.x, max.y, max.z), glm::vec3(min.x, max.y, min.z));
		cubeTriangles.emplace_back(glm::vec3(min.x, max.y, max.z), glm::vec3(max.x, max.y, max.z), glm::vec3(min.x, max.y, min.z));

		// bottom side
		cubeTriangles.emplace_back(glm::vec3(max.x, min.y, max.z), glm::vec3(max.x, min.y, min.z), glm::vec3(min.x, min.y, max.z));
		cubeTriangles.emplace_back(glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, min.y, min.z), glm::vec3(min.x, min.y, max.z));
	
		return Mesh{ std::move(cubeTriangles) };
	}

} // mesh

} // engn