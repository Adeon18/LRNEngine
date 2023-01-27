#include "Mesh.hpp"

namespace engn {

	namespace model {
		// Bounding Box
		XMVECTOR BoundingBox::size() const { return max - min; }
		XMVECTOR BoundingBox::center() const { return (min + max) / 2.f; }
		float BoundingBox::radius() const { return XMVectorGetX(XMVector3Length(size())) / 2.f; }

		void BoundingBox::expand(const BoundingBox& other) {
			min = other.min;
			max = other.max;
		}

		bool BoundingBox::contains(const XMVECTOR& P) {
			XMFLOAT3 pF3;
			XMStoreFloat3(&pF3, P);
			return
				minF3.x <= pF3.x && pF3.x <= maxF3.x &&
				minF3.y <= pF3.y && pF3.y <= maxF3.y &&
				minF3.z <= pF3.z && pF3.z <= maxF3.z;
		}

		bool BoundingBox::contains(const XMFLOAT3& P) {
			return
				minF3.x <= P.x && P.x <= maxF3.x &&
				minF3.y <= P.y && P.y <= maxF3.y &&
				minF3.z <= P.z && P.z <= maxF3.z;
		}

		void BoundingBox::setMin(const XMVECTOR& min)
		{
			this->min = min;
			XMStoreFloat3(&minF3, this->min);
		}

		void BoundingBox::setMax(const XMVECTOR& max)
		{
			this->max = max;
			XMStoreFloat3(&maxF3, this->max);
		}

	} // model

} // engn
