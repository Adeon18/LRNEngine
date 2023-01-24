#pragma once
#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "render/d3d/d3d.hpp"

#include "CBStructs.hpp"

namespace engn {
	namespace rend {
		template<typename T>
		class ConstantBuffer {
		public:
			ConstantBuffer() {}

			void init() {
				D3D11_BUFFER_DESC desc;
				desc.Usage = D3D11_USAGE_DYNAMIC; // We want to dynamically change it
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				desc.ByteWidth = static_cast<UINT>(sizeof(T) + 16 - (sizeof(T) % 16)); // 16 byte aligned, maybe edit later style
				desc.StructureByteStride = 0;

				HRESULT hr = d3d::s_device->CreateBuffer(&desc, nullptr, m_buffer.GetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("ConstantBuffer::init::CreateBuffer fail: " + std::system_category().message(hr));
					return;
				}
			}

			//! Fill the buffer with data via map
			void fill() {
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				HRESULT hr = d3d::s_devcon->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				if (FAILED(hr)) {
					Logger::instance().logErr("ConstantBuffer::fill::Map fail: " + std::system_category().message(hr));
					return;
				}
				memcpy(mappedResource.pData, &data, sizeof(T));
				d3d::s_devcon->Unmap(m_buffer.Get(), 0);
			}

			[[nodiscard]] ID3D11Buffer* const* getBufferAddress() { return m_buffer.GetAddressOf(); }
			[[nodiscard]] T& getData() { return data; }
		private:
			T data;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		};
	} // rend
} // engn