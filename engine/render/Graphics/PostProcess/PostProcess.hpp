#pragma once

#include "render/Systems/Pipeline.hpp"
#include "render/Graphics/DXRTVs/BindableRenderTarget.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

namespace engn {
	namespace rend {
		class PostProcess {
		public:
			//! Initialize all the postprocess structs for the first time, should be done in a class that owns it
			void init();

			//! Ressolve the HDR buffer into the CURRENTLY BOUND buffer
			void ressolve(const BindableRenderTarget& src);

			//! Apply the FXAA effect
			void applyFXAA(const BindableRenderTarget& src);
		private:
			const std::wstring VS_PP_NAME = L"VSPostProcess.cso";
			const std::wstring PS_PP_NAME = L"PSPostProcess.cso";
			const std::wstring PS_FXAA_NAME = L"PSFXAA.cso";
			static constexpr float EV100_CHANGE_VALUE = 0.2f;

			//! Handles input, which is kinda BAD, and is responsible for dynamically changing the EV100 value for the test
			void handleEV100Adjustment();

			Pipeline m_pipelinePP;
			ConstantBuffer<CB_PS_HDR> m_cbufferPP;

			Pipeline m_pipelineFXAA;
			ConstantBuffer<CB_PS_HDR> m_cbufferFXAA;

			//! EV100 value that can be changed vie +/- keys by 0.5 value
			float m_ev100Exposure = -1.0f;

			bool m_initialized = false;
		};
	} // rend
} // engn