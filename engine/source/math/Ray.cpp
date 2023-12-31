#include "Ray.hpp"

#include <cmath>
#include <algorithm>

//#define TRIANGLE_CULL 0

namespace engn {

	namespace geom {

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

		void BoundingBox::setMin(const XMFLOAT3& min)
		{
			this->minF3 = min;
			this->min = XMLoadFloat3(&min);
		}

		void BoundingBox::setMax(const XMVECTOR& max)
		{
			this->max = max;
			XMStoreFloat3(&maxF3, this->max);
		}

		void BoundingBox::setMax(const XMFLOAT3& max)
		{
			this->maxF3 = max;
			this->max = XMLoadFloat3(&max);
		}

		void Ray::transform(const XMMATRIX& transMat) {
			origin = XMVector3Transform(origin, transMat);
			direction = XMVector4Transform(XMVectorSetW(direction, 0.0f), transMat);
		}

		XMVECTOR Ray::getPointAt(float t) const { return origin + t * direction; }

		bool Ray::intersect(float nearest, const geom::BoundingBox& box) const
		{
			XMFLOAT3 min = box.getMinF3();
			XMFLOAT3 max = box.getMaxF3();

			XMFLOAT3 rOrigin;
			XMFLOAT3 rDirection;

			XMStoreFloat3(&rOrigin, origin);
			XMStoreFloat3(&rDirection, direction);

			// x values
			float txMin = (min.x - rOrigin.x) / rDirection.x;
			float txMax = (max.x - rOrigin.x) / rDirection.x;

			float tMin = (std::min)(txMax, txMin);
			float tMax = (std::max)(txMax, txMin);

			// y values
			float tyMin = (min.y - rOrigin.y) / rDirection.y;
			float tyMax = (max.y - rOrigin.y) / rDirection.y;

			if (tyMin > tyMax) { std::swap(tyMin, tyMax); }

			if ((tMin > tyMax) || (tyMin > tMax)) { return false; }

			tMin = (std::max)(tMin, tyMin);
			tMax = (std::min)(tMax, tyMax);

			// z values
			float tzMin = (min.z - rOrigin.z) / rDirection.z;
			float tzMax = (max.z - rOrigin.z) / rDirection.z;

			if (tzMin > tzMax) { std::swap(tzMin, tzMax); }

			if ((tMin > tzMax) || (tzMin > tMax))
				return false;

			if (tzMin > tMin)
				tMin = tzMin;

			if (tzMax < tMax)
				tMax = tzMax;

			//hitEntry.rayT = tMin;

			return true;
		}

		bool Ray::intersect(geom::MeshIntersection& nearest, const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2) const
		{

			XMVECTOR edge1 = v1 - v0;
			XMVECTOR edge2 = v2 - v0;
			XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

			XMVECTOR pv = XMVector3Cross(direction, edge2);

			float det = XMVectorGetX(XMVector3Dot(edge1, pv));

#ifdef TRIANGLE_CULL
			// If ray is paralell to the triangle
			if (det < EPS) {
				return false;
			}

			float invDet = 1.0f / det;

			XMVECTOR toRayOrigin = origin - v0;

			float uParam = XMVectorGetX(XMVector3Dot(toRayOrigin, pv));
			if (uParam < 0.0f || uParam > det) {
				return false;
			}

			XMVECTOR vParamTestVec = XMVector3Cross(toRayOrigin, edge1);

			float vParam = XMVectorGetX(XMVector3Dot(direction, vParamTestVec));
			if (vParam < 0.0f || uParam + vParam > det) {
				return false;
			}

			float t = XMVectorGetX(XMVector3Dot(edge1, vParamTestVec)) * invDet;
#else
			// If ray is paralell to the triangle
			if (det < EPS && det > -EPS) {
				return false;
			}

			float invDet = 1.0f / det;

			XMVECTOR toRayOrigin = origin - v0;

			float uParam = XMVectorGetX(XMVector3Dot(toRayOrigin, pv)) * invDet;
			if (uParam < 0.0f || uParam > 1.0f) {
				return false;
			}

			XMVECTOR vParamTestVec = XMVector3Cross(toRayOrigin, edge1);

			float vParam = XMVectorGetX(XMVector3Dot(direction, vParamTestVec)) * invDet;
			if (vParam < 0.0f || uParam + vParam > 1.0f) {
				return false;
			}

			float t = XMVectorGetX(XMVector3Dot(edge2, vParamTestVec)) * invDet;

#endif
			if (t <= 0.0f || t >= nearest.t) { return false; }

			nearest.t = t;
			nearest.pos = getPointAt(t);
			nearest.normal = (XMVectorGetX(XMVector3Dot(toRayOrigin, normal)) > 0.0f) ? normal : -normal;

			return true;
		}

		bool Ray::intersect(BasicRayIntersection& nearest, XMVECTOR normal, XMVECTOR point)
		{
			float res = XMVectorGetX(XMVector3Dot(direction, normal));

			if (res > MIN_ANGLE_DOT || res < -MIN_ANGLE_DOT)
			{
				float dist = XMVectorGetX(XMVector3Dot(point - origin, normal)) / res;

				if (dist > 0.0f && dist < nearest.t)
				{
					nearest.t = dist;
					nearest.pos = getPointAt(dist);
					// I don't have any clue how this normal works but it does and I'm happy
					nearest.normal = XMVector3Normalize(normal * -res);
					return true;
				}
			}

			return false;
		}

	} // geom

} // engn