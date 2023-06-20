#include "BindableRenderTarget.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		void BindableRenderTarget::init(int screenWidth, int screenHeight, DXGI_FORMAT textureFormat)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = screenWidth;
			textureDesc.Height = screenHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = textureFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

			HRESULT hr = d3d::s_device->CreateTexture2D(&textureDesc, nullptr, m_texture.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed at creation of RTV texture");
				return;
			}

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = textureFormat;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			hr = d3d::s_device->CreateRenderTargetView(m_texture.Get(), &rtvDesc, m_renderTargetView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed at creation of RTV");
				return;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = textureFormat;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			hr = d3d::s_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_shaderResourceView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed at creation of SRV");
				return;
			}

		}
		void BindableRenderTarget::init()
		{
			// Before calling init have to fill texture manually from the outside
			if (!m_texture.Get()) {
				Logger::instance().logErr("BindableRenderTarget::init: you need to fill texture of LDR RTV before calling init");
				return;
			}

			HRESULT hr = d3d::s_device->CreateRenderTargetView(m_texture.Get(), nullptr, m_renderTargetView.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed creating LDR RTV " + std::system_category().message(hr));
				return;
			}
		}
		void BindableRenderTarget::init(D3D11_TEXTURE2D_DESC pDesc)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = pDesc.Width;
			textureDesc.Height = pDesc.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = pDesc.Format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

			HRESULT hr = d3d::s_device->CreateTexture2D(&textureDesc, nullptr, m_texture.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed at creation of RTV texture");
				return;
			}

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = textureDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			hr = d3d::s_device->CreateRenderTargetView(m_texture.Get(), &rtvDesc, m_renderTargetView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed at creation of RTV");
				return;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			hr = d3d::s_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_shaderResourceView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed at creation of SRV");
				return;
			}
		}
		void BindableRenderTarget::OMSetCurrent(ID3D11DepthStencilView* depthStensilView)
		{
			d3d::s_devcon->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), depthStensilView);
		}
		void BindableRenderTarget::bindSRV(int slot) const
		{
			d3d::s_devcon->PSSetShaderResources(slot, 1, m_shaderResourceView.GetAddressOf());
		}
		void BindableRenderTarget::clear(const FLOAT* color)
		{
			d3d::s_devcon->ClearRenderTargetView(m_renderTargetView.Get(), color);
		}
		void BindableRenderTarget::releaseAll()
		{
			m_texture.Reset();
			m_renderTargetView.Reset();
			m_shaderResourceView.Reset();
		}
		ID3D11Texture2D* BindableRenderTarget::getTexturePtr() { return m_texture.Get(); }
		ID3D11Texture2D** BindableRenderTarget::getTexturePtrAddress() { return m_texture.GetAddressOf(); }
		ID3D11RenderTargetView* BindableRenderTarget::getRTVPtr() { return m_renderTargetView.Get(); }
		ID3D11RenderTargetView** BindableRenderTarget::getRTVPtrAddress() { return m_renderTargetView.GetAddressOf(); }

		ID3D11ShaderResourceView** BindableRenderTarget::getSRVPtrAddress()
		{
			return m_shaderResourceView.GetAddressOf();
		}

		void BindableRenderTarget::getTextureDesc(D3D11_TEXTURE2D_DESC* descToFill)
		{
			m_texture->GetDesc(descToFill);
		}
	} // rend
} // engn