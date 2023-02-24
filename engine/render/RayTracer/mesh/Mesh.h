#pragma once

#include <vector>

#include <glm/glm/glm.hpp>

#include "render/RayTracer/math/ray.h"
#include "render/RayTracer/math/geometry/triangle.h"

namespace engn {

	namespace mesh {

		//! A bounding box class for faster mesh collision detection
		struct BoundingBox {
		public:
			static constexpr float Inf = std::numeric_limits<float>::infinity();
			static constexpr BoundingBox empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
			static constexpr BoundingBox unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

			//! Get size by diagonal
			glm::vec3 size() const;
			//! Get center point of box
			glm::vec3 center() const;
			//! Get the radius of a outside sphere
			float radius() const;

			//! Change the BBs size
			void expand(const BoundingBox& other);
			//! Check if BB contains a point
			bool contains(const glm::vec3& P);

			bool hit(const math::ray& r, math::HitEntry& hitEntry) const;
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

			Mesh(const std::vector<math::triangle>& ts, const glm::vec3& min, const glm::vec3& max);

			Mesh(std::vector<math::triangle>&& ts, const glm::vec3& min, const glm::vec3& max);
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