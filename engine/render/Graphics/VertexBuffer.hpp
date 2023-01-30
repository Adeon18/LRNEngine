#pragma once

#include <iostream>
#include <vector>

#include "utils/Logger/Logger.hpp"

#include "render/d3d/d3d.hpp"


namespace engn {
	namespace rend {
		template<typename T>
		class VertexBuffer {
		public:
			VertexBuffer() {
				m_stride = sizeof(T);
			}

			VertexBuffer(const VertexBuffer<T>& other) {
				this->m_buffer = other.m_buffer;
				this->m_bufferSize = other.m_bufferSize;
				this->m_stride = other.m_stride;
			}

			VertexBuffer<T>& operator=(const VertexBuffer<T>& other) {
				this->m_buffer = other.m_buffer;
				this->m_bufferSize = other.m_bufferSize;
				this->m_stride = other.m_stride;
				return *this;
			}

			void init(const std::vector<T>& vertices) {
				m_bufferSize = static_cast<uint32_t>(vertices.size());
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
				HRESULT hr = d3d::s_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_buffer.ReleaseAndGetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("VertexBuffer::init::CreateBuffer fail: " + std::system_category().message(hr));
					return;
				}
			}

			void bind() {
				uint32_t offset = 0;
				d3d::s_devcon->IASetVertexBuffers(
					0, // slot
					1, // number of buffers
					m_buffer.GetAddressOf(),
					&m_stride,
					&offset
				);
			}

			[[nodiscard]] ID3D11Buffer* const* getBufferAddress() { return m_buffer.GetAddressOf(); }
			[[nodiscard]] uint32_t getBufferSize() const { return m_bufferSize; }
			[[nodiscard]] const uint32_t* getStride() const { return &m_stride; }
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			uint32_t m_bufferSize = 0;
			uint32_t m_stride;
		};
	} // rend
} // engn