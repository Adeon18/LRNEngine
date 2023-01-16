#include <iostream>

#include "d3d.hpp"


// Say to NVidia or AMD driver to prefer a dedicated GPU instead of an integrated.
// This has effect on laptops.
extern "C"
{
	_declspec(dllexport) uint32_t NvOptimusEnablement = 1;
	_declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}



namespace engn {
	namespace d3d {
		// global pointers to most used D3D11 objects for convenience:
		ID3D11Device5* s_device = nullptr;
		ID3D11DeviceContext4* s_devcon = nullptr;
		IDXGIFactory5* s_factory = nullptr;
	} // d3d


	void D3D::init()
	{
		HRESULT result;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)m_factory.reset());

		result = m_factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)m_factory5.reset());

		{
			uint32_t index = 0;
			IDXGIAdapter1* adapter;
			while (m_factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				std::cout << "GPU #" << index << desc.Description << std::endl;
			}
		}

		// Init D3D Device & Context

		const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
		result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
			&featureLevelRequested, 1, D3D11_SDK_VERSION, m_device.reset(), &featureLevelInitialized, m_devcon.reset());

		result = m_device->QueryInterface(__uuidof(ID3D11Device5), (void**)m_device5.reset());


		result = m_devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_devcon4.reset());


		result = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)m_devdebug.reset());


		// Write global pointers

		d3d::s_factory = m_factory5.ptr();
		d3d::s_device = m_device5.ptr();
		d3d::s_devcon = m_devcon4.ptr();
	}

} // engn