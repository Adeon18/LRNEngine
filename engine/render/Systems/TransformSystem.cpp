#include "TransformSystem.hpp"

namespace engn {
	namespace rend {
		XMMATRIX& TransformSystem::getMatrixById(uint32_t Id)
		{
			return m_transformMatrices[Id];
		}
		void TransformSystem::addMatrixById(const XMMATRIX& mat, uint32_t Id)
		{
			m_transformMatrices[Id] = mat;
		}
	} // rend
} // engn