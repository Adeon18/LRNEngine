#pragma once

#include "render/D3D/d3d.hpp"


namespace engn {
	namespace rend {
		class Sampler {
		public:
			//! Initialize the sampler with the respective filtering types and interpolation
			bool init(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressing);
			//! Bind the sampler to the respective slot
			void bind(uint32_t slot);
		private:
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
			bool m_initialized = false;
		};
	} // rend
} // engn