#pragma once

#include <iostream>

#include "utils/Logger/Logger.hpp"

#include "render/d3d/d3d.hpp"

#include "CBStructs.hpp"

namespace engn {
	namespace rend {
		template<typename T, uint32_t N>
		class StructuredBuffer {
		public:
			void init() {
				m_byteSize = N * sizeof(T);

				D3D11_BUFFER_DESC desc;
				desc.Usage = D3D11_USAGE_DEFAULT; // We want to dynamically change it
				desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				desc.ByteWidth = m_byteSize;
				desc.StructureByteStride = sizeof(T);

				HRESULT hr = d3d::s_device->CreateBuffer(&desc, nullptr, m_buffer.ReleaseAndGetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("StructuredBuffer::init::CreateBuffer fail: " + std::system_category().message(hr));
					return;
				}

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_UNKNOWN;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.NumElements = N;
				hr = d3d::s_device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_bufferSRV.ReleaseAndGetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("StructuredBuffer::init::CreateShaderResourceView fail: " + std::system_category().message(hr));
					return;
				}

				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = 0;
				uavDesc.Buffer.NumElements = N;
				uavDesc.Buffer.Flags = 0;

				hr = d3d::s_device->CreateUnorderedAccessView(m_buffer.Get(), &uavDesc, m_bufferUAV.GetAddressOf());
				if (FAILED(hr)) {
					Logger::instance().logErr("StructuredBuffer::init::CreateUnorderedAccessView fail: " + std::system_category().message(hr));
					return;
				}
			}

			[[nodiscard]] ID3D11ShaderResourceView** getSRVPtrAddress() { return m_bufferSRV.GetAddressOf(); };
			[[nodiscard]] ID3D11UnorderedAccessView** getUAVPtrAddress() { return m_bufferUAV.GetAddressOf(); };
			[[nodiscard]] ID3D11UnorderedAccessView* getUAVPtr() { return m_bufferUAV.Get(); };

		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bufferSRV;
			Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_bufferUAV;
			uint32_t m_size = N;
			uint32_t m_byteSize;
			uint32_t m_stride = sizeof(T);
			//D3D11_MAPPED_SUBRESOURCE m_mappedBuffer{};
		};
	} // rend
} // engn