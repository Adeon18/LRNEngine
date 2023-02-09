#include <iostream>

#include "IndexBuffer.hpp"


namespace engn {
	namespace rend {
		void IndexBuffer::init(const std::vector<DWORD>& indices) {
			m_bufferSize = static_cast<UINT>(indices.size());
			// Create index buffer description
			D3D11_BUFFER_DESC indexBufferDesc{};

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(DWORD) * indices.size(); // The size of buffer
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // We tell that thi sbuffer is a indexBuffer
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			// Subresource data just has a pointer to out verticess
			D3D11_SUBRESOURCE_DATA indexBufferData{};
			indexBufferData.pSysMem = indices.data();

			// Create Buffer
			HRESULT hr = d3d::s_device->CreateBuffer(&indexBufferDesc, &indexBufferData, m_buffer.GetAddressOf());
			if (FAILED(hr)) { Logger::instance().logErr("IndexBuffer::init::CreateBuffer fail: " + std::system_category().message(hr)); }
		}
	} // rend
} // engn