#pragma once

#include <array>

#include "render/D3D/d3d.hpp"

#include "render/Graphics/DXRTVs/BindableRenderTarget.hpp"

namespace engn {
	namespace rend {
		struct GBuffer {
			rend::BindableRenderTarget albedo;
			rend::BindableRenderTarget normals;
			// Used for decals
			rend::BindableRenderTarget normalsCopy;
			rend::BindableRenderTarget roughMet;
			rend::BindableRenderTarget emission;
			rend::BindableRenderTarget ids;

			std::array<ID3D11RenderTargetView*, 5> rtvPtrs;

			bool isValid = false;

			void init(int screenWidth, int screenHeight) {
				albedo.init(screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
				normals.init(screenWidth, screenHeight, DXGI_FORMAT_R16G16B16A16_SNORM);
				normalsCopy.init(screenWidth, screenHeight, DXGI_FORMAT_R16G16B16A16_SNORM);
				roughMet.init(screenWidth, screenHeight, DXGI_FORMAT_R8G8_UNORM);
				emission.init(screenWidth, screenHeight, DXGI_FORMAT_R16G16B16A16_FLOAT);
				ids.init(screenWidth, screenHeight, DXGI_FORMAT_R32_UINT);
				isValid = true;

				rtvPtrs[0] = albedo.getRTVPtr();
				rtvPtrs[1] = normals.getRTVPtr();
				rtvPtrs[2] = roughMet.getRTVPtr();
				rtvPtrs[3] = emission.getRTVPtr();
				rtvPtrs[4] = ids.getRTVPtr();
			}

			void bind(ID3D11DepthStencilView* depthStensilView) {
				d3d::s_devcon->OMSetRenderTargets(5, rtvPtrs.data(), depthStensilView);
			}

			void copyNormalsTexture() {
				d3d::s_devcon->CopyResource(normalsCopy.getTexturePtr(), normals.getTexturePtr());
			}

			void unbindIds(ID3D11DepthStencilView* depthStensilView) {
				ID3D11RenderTargetView* rTargets[5] = { albedo.getRTVPtr(), normals.getRTVPtr(), roughMet.getRTVPtr(), emission.getRTVPtr(), NULL };
				d3d::s_devcon->OMSetRenderTargets(5, rTargets, depthStensilView);
			}

			void unBind(ID3D11DepthStencilView* depthStensilView) {
				ID3D11RenderTargetView* rTargets[5] = { NULL, NULL, NULL, NULL, NULL };
				d3d::s_devcon->OMSetRenderTargets(5, rTargets, depthStensilView);
			}

			void clear(float* color) {
				albedo.clear(color);
				normals.clear(color);
				normalsCopy.clear(color);
				roughMet.clear(color);
				emission.clear(color);
				ids.clear(color);
			}

			void reset() {
				albedo.releaseAll();
				normals.releaseAll();
				normalsCopy.releaseAll();
				roughMet.releaseAll();
				emission.releaseAll();
				ids.releaseAll();

				rtvPtrs.fill(nullptr);
			}
		};
	} // rend
} // engn