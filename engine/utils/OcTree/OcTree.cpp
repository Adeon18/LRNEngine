#include <iostream>
#include <algorithm>

#include "utility/utility.hpp"

#include "OcTree.hpp"


namespace engn {
	namespace util {
		const int TriangleOctree::PREFFERED_TRIANGLE_COUNT = 32;
		const float TriangleOctree::MAX_STRETCHING_RATIO = 1.05f;

		inline const XMVECTOR getPos(const mdl::Mesh& mesh, uint32_t triangleIndex, uint32_t vertexIndex)
		{
			uint32_t index = mesh.triangles.empty() ?
				triangleIndex * 3 + vertexIndex :
				mesh.triangles[triangleIndex].indices[vertexIndex];

			return XMLoadFloat3(&(mesh.vertices[index].pos));
		}

		void TriangleOctree::initialize(const mdl::Mesh& mesh)
		{
			m_triangles.clear();
			m_triangles.shrink_to_fit();

			m_mesh = &mesh;
			m_children = nullptr;

			const XMVECTOR eps = { 1e-5f, 1e-5f, 1e-5f };
			m_box = m_initialBox = { mesh.box.getMin() - eps, mesh.box.getMax() + eps };

			for (uint32_t i = 0; i < mesh.triangles.size(); ++i)
			{
				const XMVECTOR V1 = getPos(mesh, i, 0);
				const XMVECTOR V2 = getPos(mesh, i, 1);
				const XMVECTOR V3 = getPos(mesh, i, 2);

				XMVECTOR P = (V1 + V2 + V3) / 3.f;

				bool inserted = addTriangle(i, V1, V2, V3, P);
			}

			std::cout << m_mesh->name + ": triangles: " << m_triangles.size() << std::endl;
		}

		void TriangleOctree::initialize(const mdl::Mesh& mesh, const mdl::BoundingBox& parentBox, const XMVECTOR& parentCenter, int octetIndex)
		{
			m_mesh = &mesh;
			m_children = nullptr;

			const float eps = 1e-5f;

			XMFLOAT3 initBoxMin{};
			XMFLOAT3 initBoxMax{};

			if (octetIndex % 2 == 0)
			{
				initBoxMin.x = XMVectorGetX(parentBox.getMin());
				initBoxMax.x = XMVectorGetX(parentCenter);
			}
			else
			{
				initBoxMin.x = XMVectorGetX(parentCenter);
				initBoxMax.x = XMVectorGetX(parentBox.getMax());
			}

			if (octetIndex % 4 < 2)
			{
				initBoxMin.y = XMVectorGetY(parentBox.getMin());
				initBoxMax.y = XMVectorGetY(parentCenter);
			}
			else
			{
				initBoxMin.y = XMVectorGetY(parentCenter);
				initBoxMax.y = XMVectorGetY(parentBox.getMax());
			}

			if (octetIndex < 4)
			{
				initBoxMin.z = XMVectorGetZ(parentBox.getMin());
				initBoxMax.z = XMVectorGetZ(parentCenter);
			}
			else
			{
				initBoxMin.z = XMVectorGetZ(parentCenter);
				initBoxMax.z = XMVectorGetZ(parentBox.getMax());
			}
			m_initialBox.setMin(initBoxMin);
			m_initialBox.setMax(initBoxMax);

			m_box = m_initialBox;
			XMVECTOR elongation = (MAX_STRETCHING_RATIO - 1.f) * m_box.size();

			if (octetIndex % 2 == 0) initBoxMax.x += XMVectorGetX(elongation);
			else initBoxMin.x -= XMVectorGetX(elongation);

			if (octetIndex % 4 < 2) initBoxMax.y += XMVectorGetY(elongation);
			else initBoxMin.y -= XMVectorGetY(elongation);

			if (octetIndex < 4) initBoxMax.z += XMVectorGetZ(elongation);
			else initBoxMin.z -= XMVectorGetZ(elongation);

			m_box.setMin(initBoxMin);
			m_box.setMax(initBoxMax);
		}

		bool TriangleOctree::addTriangle(uint32_t triangleIndex, const XMVECTOR& V1, const XMVECTOR& V2, const XMVECTOR& V3, const XMVECTOR& center)
		{
			if (!m_initialBox.contains(center) ||
				!m_box.contains(V1) ||
				!m_box.contains(V2) ||
				!m_box.contains(V3))
			{
				return false;
			}

			if (m_children == nullptr)
			{
				if (m_triangles.size() < PREFFERED_TRIANGLE_COUNT)
				{
					m_triangles.emplace_back(triangleIndex);
					return true;
				}
				else
				{
					XMVECTOR C = (m_initialBox.getMin() + m_initialBox.getMax()) / 2.f;

					m_children.reset(new std::array<TriangleOctree, 8>());
					for (int i = 0; i < 8; ++i)
					{
						(*m_children)[i].initialize(*m_mesh, m_initialBox, C, i);
					}

					std::vector<uint32_t> newTriangles;

					for (uint32_t index : m_triangles)
					{
						const XMVECTOR& P1 = getPos(*m_mesh, index, 0);
						const XMVECTOR& P2 = getPos(*m_mesh, index, 1);
						const XMVECTOR& P3 = getPos(*m_mesh, index, 2);

						XMVECTOR P = (P1 + P2 + P3) / 3.f;

						int i = 0;
						for (; i < 8; ++i)
						{
							if ((*m_children)[i].addTriangle(index, P1, P2, P3, P))
								break;
						}

						if (i == 8)
							newTriangles.emplace_back(index);
					}

					m_triangles = std::move(newTriangles);
				}
			}

			int i = 0;
			for (; i < 8; ++i)
			{
				if ((*m_children)[i].addTriangle(triangleIndex, V1, V2, V3, center))
					break;
			}

			if (i == 8)
				m_triangles.emplace_back(triangleIndex);

			return true;
		}

		bool TriangleOctree::intersect(const geom::Ray& ray, mdl::MeshIntersection& nearest) const
		{
			float boxT = nearest.t;
			if (!ray.intersect(boxT, m_box)) {
				/*std::cout << "Box min: " << m_box.getMin() << std::endl;
				std::cout << "Box max: " << m_box.getMax() << std::endl;
				std::cout << "Ray T: " << boxT << std::endl;*/
				return false;
			}

			return intersectInternal(ray, nearest);
		}

		bool TriangleOctree::intersectInternal(const geom::Ray& ray, mdl::MeshIntersection& nearest) const
		{
			{
				float boxT = nearest.t;
				if (!ray.intersect(boxT, m_box)) {
					return false;
				}
			}

			bool found = false;

			for (uint32_t i = 0; i < m_triangles.size(); ++i)
			{
				const XMVECTOR& V1 = getPos(*m_mesh, m_triangles[i], 0);
				const XMVECTOR& V2 = getPos(*m_mesh, m_triangles[i], 1);
				const XMVECTOR& V3 = getPos(*m_mesh, m_triangles[i], 2);
				// I EDITED
				if (ray.intersect(nearest, V1, V2, V3))
				{
					std::cout << "AAAAAAAAAAA" << std::endl;
					std::cout << "V1: " << V1 << std::endl;
					std::cout << "V2: " << V2 << std::endl;
					std::cout << "V3: " << V3 << std::endl;
					nearest.triangle = i;
					found = true;
				}
			}

			if (!m_children) return found;

			struct OctantIntersection
			{
				int index;
				float t;
			};

			std::array<OctantIntersection, 8> boxIntersections;

			for (int i = 0; i < 8; ++i)
			{
				if ((*m_children)[i].m_box.contains(ray.origin))
				{
					boxIntersections[i].index = i;
					boxIntersections[i].t = 0.f;
				}
				else
				{
					float boxT = nearest.t;
					if (ray.intersect(boxT, (*m_children)[i].m_box))
					{
						boxIntersections[i].index = i;
						boxIntersections[i].t = boxT;
					}
					else
					{
						boxIntersections[i].index = -1;
					}
				}
			}

			std::sort(boxIntersections.begin(), boxIntersections.end(),
				[](const OctantIntersection& A, const OctantIntersection& B) -> bool { return A.t < B.t; });

			for (int i = 0; i < 8; ++i)
			{
				if (boxIntersections[i].index < 0 || boxIntersections[i].t > nearest.t)
					continue;

				if ((*m_children)[boxIntersections[i].index].intersectInternal(ray, nearest))
				{
					found = true;
				}
			}

			return found;
		}
	} // util
} // engn
