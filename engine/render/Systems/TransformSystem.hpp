#pragma once

#include <unordered_map>

#include "render/D3D/d3d.hpp"

#include "utils/SolidVector/SolidVector.hpp"

namespace engn {
	namespace rend {
		class TransformSystem {
		public:
			TransformSystem(const TransformSystem& other) = delete;
			TransformSystem& operator=(const TransformSystem& other) = delete;

			static TransformSystem& getInstance() {
				static TransformSystem t;
				return t;
			}
			//! Get the REFERENCE to a matrix in transformsystem by ID
			XMMATRIX& getMatrixById(uint32_t Id);
			XMMATRIX getMatrixByIdCopy(uint32_t Id);
			//! Insert a new transform matrix by index
			uint32_t addMatrixById(const XMMATRIX& mat);
			//! Erase matrix given the ID
			void eraseMatrixById(uint32_t Id);
		private:
			TransformSystem() {}

			//std::unordered_map<uint32_t, XMMATRIX> m_transformMatrices;
			util::SolidVector<XMMATRIX> m_transformMatrices;
		};
	}
} // engn