#include "BindableDepthBuffer.hpp"

namespace engn {
	namespace rend {
		void BindableDepthBuffer::init(
			int screenWidth,
			int screenHeight,
			DXGI_FORMAT textureFormat,
			DXGI_FORMAT DSVformat,
			DXGI_FORMAT SRVFormat,
			bool isTextureCube
		)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = screenWidth;
			textureDesc.Height = screenHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = (isTextureCube) ? 6 : 1;
			textureDesc.Format = textureFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			if (isTextureCube) { textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; }

			HRESULT hr = d3d::s_device->CreateTexture2D(&textureDesc, nullptr, m_texture.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableDepthBuffer::init: Failed at creation depth texture");
				return;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
			depthStencilViewDesc.Format = DSVformat;
			depthStencilViewDesc.ViewDimension = (isTextureCube) ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			hr = d3d::s_device->CreateDepthStencilView(m_texture.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableDepthBuffer::init: Failed at creation of DSV");
				return;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
			shaderResourceViewDesc.Format = SRVFormat;
			shaderResourceViewDesc.ViewDimension = (isTextureCube) ? D3D11_SRV_DIMENSION_TEXTURE2DARRAY : D3D11_SRV_DIMENSION_TEXTURE2D;;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			hr = d3d::s_device->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, m_shaderResourceView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableDepthBuffer::init: Failed at creation of SRV");
				return;
			}

		}
		void BindableDepthBuffer::bindSRV(int slot) const
		{
			d3d::s_devcon->PSSetShaderResources(slot, 1, m_shaderResourceView.GetAddressOf());
		}
		void BindableDepthBuffer::clear()
		{
			d3d::s_devcon->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
		}
		void BindableDepthBuffer::releaseAll()
		{
			m_texture.Reset();
			m_depthStencilView.Reset();
			m_shaderResourceView.Reset();
		}
		ID3D11Texture2D* BindableDepthBuffer::getTexturePtr()
		{
			return m_texture.Get();;
		}
		ID3D11Texture2D** BindableDepthBuffer::getTexturePtrAddress()
		{
			return m_texture.GetAddressOf();
		}

		ID3D11DepthStencilView* BindableDepthBuffer::getDSVPtr()
		{
			return m_depthStencilView.Get();
		}

		ID3D11DepthStencilView** BindableDepthBuffer::getDSVPtrAddress()
		{
			return m_depthStencilView.GetAddressOf();
		}

	} // rend
} // engn