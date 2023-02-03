#pragma once

#include <DirectXMath.h>

#include "source/mesh/MeshHelpers.hpp"

namespace engn {

	namespace mdl {
		struct MeshIntersection;
		struct BoundingBox;
	}

	namespace geom {
		using namespace DirectX;

		struct BasicRayIntersection {
			XMVECTOR pos;
			XMVECTOR normal;
			float t;
		};

		struct Ray {
			static constexpr float EPS = 0.00001f;
			static constexpr float MIN_ANGLE_DOT = 1e-6f;

			Ray() : origin{}, direction{} {}
			Ray(const XMVECTOR& o, const XMVECTOR& d) : origin{ o }, direction{ d } {}

			//! Transform a ray by specified matrix
			void transform(const XMMATRIX& transMat);

			//! Cast a ray in the direction by t
			XMVECTOR getPointAt(float t) const;

			bool intersect(float nearest, const mdl::BoundingBox& box) const;
			bool intersect(mdl::MeshIntersection& nearest, const XMVECTOR& v1, const XMVECTOR& v2, const XMVECTOR& v3) const;
			bool intersect(BasicRayIntersection& nearest, XMVECTOR normal, XMVECTOR point);

			XMVECTOR origin;
			XMVECTOR direction;
		};

	} // geom

} // engn