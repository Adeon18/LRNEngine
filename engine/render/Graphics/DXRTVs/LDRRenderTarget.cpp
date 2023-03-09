#pragma once

#include "LDRRenderTarget.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		void LDRRenderTarget::init()
		{
			// Before calling init have to fill texture manually from the outside
			if (!m_texture.Get()) {
				Logger::instance().logErr("LDRRenderTarget::init: you need to fill texture of LDR RTV before calling init");
				return;
			}

			HRESULT hr = d3d::s_device->CreateRenderTargetView(m_texture.Get(), nullptr, m_renderTargetView.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("LDRRenderTarget::init: Failed creating LDR RTV " + std::system_category().message(hr));
				return;
			}
		}
		void LDRRenderTarget::OMSetCurrent(ID3D11DepthStencilView* depthStensilView)
		{
			d3d::s_devcon->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), depthStensilView);
		}
		void LDRRenderTarget::clear(const FLOAT* color)
		{
			d3d::s_devcon->ClearRenderTargetView(m_renderTargetView.Get(), color);
		}
		void LDRRenderTarget::releaseAll()
		{
			m_texture.Reset();
			m_renderTargetView.Reset();
		}
	} // rend
} // engn