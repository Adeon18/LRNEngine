#include "Ray.hpp"

namespace engn {

	namespace geom {

		void Ray::transform(const XMMATRIX& transMat) {
			origin = XMVector3Transform(origin, transMat);
		}

		XMVECTOR Ray::getPointAt(float t) const { return origin + t * direction; }

		bool Ray::intersect(float nearest, const mdl::BoundingBox& box) const
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

		bool Ray::intersect(mdl::MeshIntersection& nearest, const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2) const
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

			glm::vec3 toRayOrigin = r.origin - vertices[0];

			float uParam = glm::dot(toRayOrigin, pv);
			if (uParam < 0.0f || uParam > det) {
				return false;
			}

			glm::vec3 vParamTestVec = glm::cross(toRayOrigin, edges[0]);

			float vParam = glm::dot(r.direction, vParamTestVec);
			if (vParam < 0.0f || uParam + vParam > det) {
				return false;
			}

			float t = glm::dot(edges[1], vParamTestVec) * invDet;
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

	} // geom

} // engn