#include "TransformSystem.hpp"

namespace engn {
	namespace rend {
		XMMATRIX& TransformSystem::getMatrixById(uint32_t Id)
		{
			return m_transformMatrices[Id];
		}
		uint32_t TransformSystem::addMatrixById(const XMMATRIX& mat)
		{
			return m_transformMatrices.insert(mat);
		}
	} // rend
} // engn