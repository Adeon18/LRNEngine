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

	namespace rend {
		void D3D::init()
		{
			HRESULT result;

			// Manage video adapters
			result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)m_factory.reset());
			if (FAILED(result)) { std::cout << "CreateDXGIFactory fail" << std::endl; }

			result = m_factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)m_factory5.reset());
			if (FAILED(result)) { std::cout << "m_factory->QueryInterface fail" << std::endl; }

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
			result = D3D11CreateDevice(
				nullptr, // Adapter ptr
				D3D_DRIVER_TYPE_HARDWARE, // Adapter type, HW if Adapter = nulptr, UNKNOWN if we specify adapter
				nullptr, // A handle to the dll that implements software rasterizer
				D3D11_CREATE_DEVICE_DEBUG, // runtime layers => debug here
				&featureLevelRequested, // Pointer to an array of featureLevels
				1, // Number of elements in featureLayers array
				D3D11_SDK_VERSION, // Version of SDK
				m_device.reset(), // The address of the pointer to ID3D11Device -> will return the address
				&featureLevelInitialized, // If successful => return the first feature level from the feature level array which succeeded
				m_devcon.reset() // The address of the pointer to the ID3D11DeviceContext object
			);
			if (FAILED(result)) { std::cout << "D3D11CreateDevice fail" << std::endl; }
			if (featureLevelInitialized != featureLevelRequested) { std::cout << "D3D_FEATURE_LEVEL_11_0 fail" << std::endl; }

			result = m_device->QueryInterface(__uuidof(ID3D11Device5), (void**)m_device5.reset());
			if (FAILED(result)) { std::cout << "QueryInterface fail m_device5" << std::endl; }

			result = m_devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_devcon4.reset());
			if (FAILED(result)) { std::cout << "QueryInterface fail m_devcon4" << std::endl; }

			result = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)m_devdebug.reset());
			if (FAILED(result)) { std::cout << "QueryInterface fail m_devdebug" << std::endl; }


			// Write global pointers

			d3d::s_factory = m_factory5.ptr();
			d3d::s_device = m_device5.ptr();
			d3d::s_devcon = m_devcon4.ptr();
		}
		void D3D::deinit()
		{
			// Pointers are released at destruction
		}
	} // rend
} // engn