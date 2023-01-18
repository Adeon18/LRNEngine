#pragma once

//#include <iostream>
//#include <array>
//
//#include "render/d3d/d3d.hpp"
//

//namespace engn {
//	namespace rend {
//		template<typename T>
//		class VertexBuffer {
//		public:
//			VertexBuffer() {}
//
//			void init(const std::array<T>& vertices) {
//				// Create vertex buffer description
//				D3D11_BUFFER_DESC vertexBufferDesc{};
//
//				vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//				vertexBufferDesc.ByteWidth = sizeof(T) * vertices.size(); // The size of buffer
//				vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // We tell that thi sbuffer is a vertexbuffer
//				vertexBufferDesc.CPUAccessFlags = 0;
//				vertexBufferDesc.MiscFlags = 0;
//				vertexBufferDesc.StructureByteStride = 0;
//
//				// Subresource data just has a pointer to out vertices
//				D3D11_SUBRESOURCE_DATA vertexBufferData{};
//				vertexBufferData.pSysMem = vertices.data();
//
//				// Create Buffer
//				HRESULT res = d3d::s_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_buffer.reset());
//				if (FAILED(res)) { std::cout << "CreateBuffer fail" << std::endl; }
//			}
//		private:
//			DxResPtr<ID3D11Buffer> m_buffer;
//		};
//	} // rend
//} // engn