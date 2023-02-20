#include "Sampler.hpp"

#include "utils/Logger/Logger.hpp"

namespace engn {
	namespace rend {
		bool Sampler::init(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressing)
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = filter;
			samplerDesc.AddressU = addressing;
			samplerDesc.AddressV = addressing;
			samplerDesc.AddressW = addressing;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			HRESULT hr = d3d::s_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logWarn("Sampler::init::CreateSampler fail");
				return false;
			}
			m_initialized = true;
			return true;
		}

		void Sampler::bind(uint32_t slot)
		{
			if (!m_initialized) {
				Logger::instance().logWarn("Can't bind sampler because it is not initialized, slot: " + std::to_string(slot));
				return;
			}
			d3d::s_devcon->PSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
		}
	} // rend
} // engn