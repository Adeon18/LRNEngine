#pragma once

#include <unordered_map>

#include "render/D3D/d3d.hpp"

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
			//! Add a new transform matrix by index
			void addMatrixById(const XMMATRIX& mat, uint32_t Id);
		private:
			TransformSystem() {}

			std::unordered_map<uint32_t, XMMATRIX> m_transformMatrices;
		};
	}
} // engn