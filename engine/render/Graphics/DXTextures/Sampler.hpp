#pragma once

#include "render/D3D/d3d.hpp"


namespace engn {
	namespace rend {
		class Sampler {
		public:
			//! Initialize the sampler with the respective filtering types and interpolation
			//! Comparison is set to NEVER by defualt
			bool init(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressing, D3D11_COMPARISON_FUNC compFunc = D3D11_COMPARISON_NEVER);
			//! Bind the sampler to the respective slot
			void bind(uint32_t slot);
		private:
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
			bool m_initialized = false;
		};
	} // rend
} // engn