#include "HDRRenderTarget.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		void HDRRenderTarget::init(int screenWidth, int screenHeight)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = screenWidth;
			textureDesc.Height = screenHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

			HRESULT hr = d3d::s_device->CreateTexture2D(&textureDesc, nullptr, m_texture.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("HDRRenderTarget::init: Failed at creation of RTV texture");
				return;
			}

			hr = d3d::s_device->CreateRenderTargetView(m_texture.Get(), nullptr, m_renderTargetView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("HDRRenderTarget::init: Failed at creation of RTV");
				return;
			}

			hr = d3d::s_device->CreateShaderResourceView(m_texture.Get(), nullptr, m_shaderResourceView.GetAddressOf());

			if (FAILED(hr)) {
				Logger::instance().logErr("HDRRenderTarget::init: Failed at creation of SRV");
				return;
			}
			
		}
		void HDRRenderTarget::OMSetCurrent(ID3D11DepthStencilView* depthStensilView)
		{
			d3d::s_devcon->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), depthStensilView);
		}
		void HDRRenderTarget::bindSRV(int slot)
		{
			d3d::s_devcon->PSSetShaderResources(slot, 1, m_shaderResourceView.GetAddressOf());
		}
		void HDRRenderTarget::releaseAll()
		{
			m_texture.Reset();
			m_renderTargetView.Reset();
			m_shaderResourceView.Reset();
		}
	} // rend
} // engn