#pragma once

#include <iostream>
#include <vector>

#include "render/d3d/d3d.hpp"


namespace engn {
	namespace rend {
		template<typename T>
		class VertexBuffer {
		public:
			VertexBuffer() {
				m_stride = sizeof(T);
			}

			void init(const std::vector<T>& vertices) {
				m_bufferSize = static_cast<UINT>(vertices.size());
				// Create vertex buffer description
				D3D11_BUFFER_DESC vertexBufferDesc{};

				vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				vertexBufferDesc.ByteWidth = sizeof(T) * vertices.size(); // The size of buffer
				vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // We tell that thi sbuffer is a vertexbuffer
				vertexBufferDesc.CPUAccessFlags = 0;
				vertexBufferDesc.MiscFlags = 0;
				vertexBufferDesc.StructureByteStride = 0;

				// Subresource data just has a pointer to out vertices
				D3D11_SUBRESOURCE_DATA vertexBufferData{};
				vertexBufferData.pSysMem = vertices.data();

				// Create Buffer
				HRESULT res = d3d::s_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_buffer.GetAddressOf());
				if (FAILED(res)) { std::cout << "CreateBuffer fail" << std::endl; }
			}

			[[nodiscard]] ID3D11Buffer* const* getBufferAddress() { return m_buffer.GetAddressOf(); }
			[[nodiscard]] UINT getBufferSize() const { return m_bufferSize; }
			[[nodiscard]] const UINT* getStride() const { return &m_stride; }
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			UINT m_bufferSize = 0;
			UINT m_stride;
		};
	} // rend
} // engn