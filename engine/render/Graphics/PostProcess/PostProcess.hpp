#pragma once

#include "render/Systems/Pipeline.hpp"
#include "render/Graphics/DXRTVs/LDRRenderTarget.hpp"
#include "render/Graphics/DXRTVs/HDRRenderTarget.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

namespace engn {
	namespace rend {
		class PostProcess {
		public:
			//! Initialize all the postprocess structs for the first time, should be done in a class that owns it
			void init();

			//! Ressolve the HDR buffer into the LDR buffer using a postprocess effect
			void ressolve(const HDRRenderTarget& src, const LDRRenderTarget& dest);
		private:
			const std::wstring VS_NAME = L"VSPostProcess.cso";
			const std::wstring PS_NAME = L"PSPostProcess.cso";

			Pipeline m_pipeline;
			ConstantBuffer<CB_PS_HDR> m_cbuffer;

			bool m_initialized = false;
		};
	} // rend
} // engn