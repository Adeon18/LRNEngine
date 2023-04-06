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

			hr = d3d::s_device->CreateRenderTargetView(m_texture.Get(), nullptr, m_renderTargetView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("BindableRenderTarget::init: Failed at creation of RTV");
				return;
			}

			hr = d3d::s_device->CreateShaderResourceView(m_texture.Get(), nullptr, m_shaderResourceView.GetAddressOf());

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
	} // rend
} // engn