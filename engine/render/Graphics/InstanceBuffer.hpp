#pragma once
#include <iostream>

#include "Vertex.hpp"

#include "utils/Logger/Logger.hpp"

#include "render/d3d/d3d.hpp"

#include "CBStructs.hpp"

namespace engn {
	namespace rend {
		// Wery similar to the Constant buffer but not 16 byte aligned
		template<typename T>
		class InstanceBuffer {
		public:
			InstanceBuffer() {}

			void init(uint32_t size) {
				m_size = size;
				m_byteSize = size * sizeof(T);

				D3D11_BUFFER_DESC desc;
				desc.Usage = D3D11_USAGE_DYNAMIC; // We want to dynamically change it
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				desc.ByteWidth = m_byteSize;
				desc.StructureByteStride = 0;

				HRESULT hr = d3d::s_device->CreateBuffer(&desc, nullptr, m_buffer.ReleaseAndGetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("InstanceBuffer::init::CreateBuffer fail: " + std::system_category().message(hr));
					return;
				}
			}

			void bind() {
				uint32_t offset = 0;
				d3d::s_devcon->IASetVertexBuffers(
					1, // slot
					1, // number of buffers
					m_buffer.GetAddressOf(),
					&m_stride,
					&offset
				);
			}

			//! Map the buffer and return the reference to the structure of mapped resource
			bool map() {
				HRESULT hr = d3d::s_devcon->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedBuffer);
				if (FAILED(hr)) {
					Logger::instance().logErr("InstanceBuffer::map: Map fail: " + std::system_category().message(hr));
					return false;
				}
				return true;
			}
			//! Unmap the buffer, must be called after we our operations on map()
			void unmap() {
				d3d::s_devcon->Unmap(m_buffer.Get(), 0);
				memset(&m_mappedBuffer, 0, sizeof(m_mappedBuffer));
			}

			//! Fill the buffer with data via map - EXPENSIVE, uses memcpy
			void fill(const std::vector<T>& buff) {
				D3D11_MAPPED_SUBRESOURCE m_mappedBuffer;
				HRESULT hr = d3d::s_devcon->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedBuffer);
				if (FAILED(hr)) {
					Logger::instance().logErr("InstanceBuffer::fill::Map fail: " + std::system_category().message(hr));
					return;
				}
				memcpy(m_mappedBuffer.pData, buff.data(), buff.size() * sizeof(T));
				d3d::s_devcon->Unmap(m_buffer.Get(), 0);
			}

			[[nodiscard]] ID3D11Buffer* const* getBufferAddress() { return m_buffer.GetAddressOf(); }
			[[nodiscard]] D3D11_MAPPED_SUBRESOURCE& getMappedBuffer() { return m_mappedBuffer; }
		private:
			uint32_t m_size;
			uint32_t m_byteSize;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

			uint32_t m_stride = sizeof(T);
			D3D11_MAPPED_SUBRESOURCE m_mappedBuffer{};
		};
	} // rend
} // engn