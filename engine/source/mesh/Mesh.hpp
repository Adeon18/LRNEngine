#pragma once

#pragma once

#include <vector>
#include <string>

#include <DirectXMath.h>

#include "render/Graphics/Vertex.hpp"

namespace engn {

	namespace mdl {
		using namespace DirectX;
		//! A bounding box class for faster mesh collision detection
		struct BoundingBox {
		public:
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
			//! Set max vectors
			void setMax(const XMVECTOR& max);

			[[nodiscard]] XMVECTOR& getMin() { return min; }
			[[nodiscard]] XMVECTOR& getMax() { return max; }
		private:
			//! Member variables
			XMVECTOR min, max;
			XMFLOAT3 minF3, maxF3;
		};

		class Mesh {
		public:
			struct Triangle {
				uint32_t indices[3];
			};
			std::string name;
			BoundingBox box = BoundingBox::empty();
			std::vector<Vertex> vertices;
			std::vector<Triangle> triangles;

			XMMATRIX meshToModel;
			XMMATRIX meshToModelInv;
			//std::vector<XMMATRIX> instances;
			//std::vector<XMMATRIX> instancesInv;
		private:
		};
	} // mdl

} // engn