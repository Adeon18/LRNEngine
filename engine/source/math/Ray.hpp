#pragma once

#include "render/Graphics/Vertex.hpp"
#include <limits>
#include <cmath>

/// <summary>
/// This header contains most of the geometry for the engine
/// </summary>

namespace engn {

	namespace geom {
		using namespace DirectX;

		struct BasicRayIntersection {
			static constexpr float MAX_DIST = 1000.0f;

			XMVECTOR pos;
			XMVECTOR normal;
			float t;

			static BasicRayIntersection empty() { return { {}, {}, MAX_DIST }; }
		};

		struct MeshIntersection
		{
			static constexpr float MAX_LENGTH = 1000.0f;
			static const MeshIntersection empty() { return { {}, {}, MAX_LENGTH }; }

			XMVECTOR pos;
			XMVECTOR normal;
			float t;

			bool valid() const { return std::isfinite(t); }
		};

		/// <summary>
		/// Bounding box geometry
		/// </summary>
		struct BoundingBox {
		public:
			BoundingBox() {
				this->min = { -1.f, -1.f, -1.f };
				this->max = { 1.f, 1.f, 1.f };
				XMStoreFloat3(&minF3, this->min);
				XMStoreFloat3(&maxF3, this->max);
			}

			BoundingBox(const XMVECTOR& min, const XMVECTOR& max) {
				this->min = min;
				this->max = max;
				XMStoreFloat3(&minF3, min);
				XMStoreFloat3(&maxF3, max);
			}

			static constexpr float Inf = std::numeric_limits<float>::infinity();
			static const BoundingBox empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
			static const BoundingBox unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

			//! Get size by diagonal
			XMVECTOR size() const;
			//! Get center point of box
			XMVECTOR center() const;
			//! Get the radius of a outside sphere
			float radius() const;

			//! Change the BBs size
			void expand(const BoundingBox& other);
			//! Check if BB contains a point
			bool contains(const XMVECTOR& P);
			bool contains(const XMFLOAT3& P);

			//! Set min vectors
			void setMin(const XMVECTOR& min);
			void setMin(const XMFLOAT3& min);
			//! Set max vectors
			void setMax(const XMVECTOR& max);
			void setMax(const XMFLOAT3& max);

			[[nodiscard]] const XMVECTOR& getMin() const { return min; }
			[[nodiscard]] const XMFLOAT3& getMinF3() const { return minF3; }
			[[nodiscard]] const XMVECTOR& getMax() const { return max; }
			[[nodiscard]] const XMFLOAT3& getMaxF3() const { return maxF3; }
		private:
			//! Member variables
			XMVECTOR min, max;
			XMFLOAT3 minF3, maxF3;
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

			bool intersect(float nearest, const geom::BoundingBox& box) const;
			bool intersect(geom::MeshIntersection& nearest, const XMVECTOR& v1, const XMVECTOR& v2, const XMVECTOR& v3) const;
			bool intersect(BasicRayIntersection& nearest, XMVECTOR normal, XMVECTOR point);

			XMVECTOR origin;
			XMVECTOR direction;
		};

	} // geom

} // engn