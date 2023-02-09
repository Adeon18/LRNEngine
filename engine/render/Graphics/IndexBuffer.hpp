#pragma once

#include <vector>

#include "utils/Logger/Logger.hpp"

#include "render/d3d/d3d.hpp"


namespace engn {
	namespace rend {
		class IndexBuffer {
		public:
			IndexBuffer() {}
			// May be improved via concepts
			void init(const std::vector<DWORD>& indices);
			
			[[nodiscard]] ID3D11Buffer* const* getBufferAddress() { return m_buffer.GetAddressOf(); }
			[[nodiscard]] ID3D11Buffer* getBufferPtr() { return m_buffer.Get(); }
			[[nodiscard]] UINT getBufferSize() const { return m_bufferSize; }
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			UINT m_bufferSize = 0;
		};
	} // rend
} // engn