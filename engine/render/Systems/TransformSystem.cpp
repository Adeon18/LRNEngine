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
		void TransformSystem::eraseMatrixById(uint32_t Id)
		{
			m_transformMatrices.erase(Id);
		}
	} // rend
} // engn