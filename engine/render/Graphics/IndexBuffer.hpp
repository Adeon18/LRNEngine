#pragma once

#include <vector>

#include "utils/Logger/Logger.hpp"

#include "render/d3d/d3d.hpp"


namespace engn {
	namespace rend {
		class IndexBuffer {
		public:
			IndexBuffer() {}

			IndexBuffer(const IndexBuffer& other) {
				this->m_buffer = other.m_buffer;
				this->m_bufferSize = other.m_bufferSize;
			}

			IndexBuffer& operator=(const IndexBuffer& other) {
				this->m_buffer = other.m_buffer;
				this->m_bufferSize = other.m_bufferSize;
				return *this;
			}
			// May be improved via concepts
			void init(const std::vector<DWORD>& indices);
			//! Bind the indexBuffer to devcon
			void bind();
			
			[[nodiscard]] ID3D11Buffer* const* getBufferAddress() { return m_buffer.GetAddressOf(); }
			[[nodiscard]] ID3D11Buffer* getBufferPtr() { return m_buffer.Get(); }
			[[nodiscard]] uint32_t getBufferSize() const { return m_bufferSize; }
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			uint32_t m_bufferSize = 0;
		};
	} // rend
} // engn