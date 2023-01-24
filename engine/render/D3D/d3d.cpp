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
			// Manage video adapters
			HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(m_factory.GetAddressOf()));
			if (FAILED(hr)) { Logger::instance().logErr("CreateDXGIFactory fail: " + std::system_category().message(hr)); }

			hr = m_factory.As(&m_factory5);
			if (FAILED(hr)) { Logger::instance().logErr("Factory Interface generation fail: " + std::system_category().message(hr)); }

			{
				uint32_t index = 0;
				IDXGIAdapter1* adapter;
				while (m_factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
				{
					DXGI_ADAPTER_DESC1 desc;
					adapter->GetDesc1(&desc);

					Logger::instance().logInfo(std::wstring{ L"GPU Desc: " } + desc.Description + L" #", index);
				}
			}

			// Init D3D Device & Context
			const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
			D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0; 
			hr = D3D11CreateDevice(
				nullptr, // Adapter ptr
				D3D_DRIVER_TYPE_HARDWARE, // Adapter type, HW if Adapter = nulptr, UNKNOWN if we specify adapter
				nullptr, // A handle to the dll that implements software rasterizer
				D3D11_CREATE_DEVICE_DEBUG, // runtime layers => debug here
				&featureLevelRequested, // Pointer to an array of featureLevels
				1, // Number of elements in featureLayers array
				D3D11_SDK_VERSION, // Version of SDK
				m_device.GetAddressOf(), // The address of the pointer to ID3D11Device -> will return the address
				&featureLevelInitialized, // If successful => return the first feature level from the feature level array which succeeded
				m_devcon.GetAddressOf() // The address of the pointer to the ID3D11DeviceContext object
			);
			if (FAILED(hr)) { Logger::instance().logErr("D3D11CreateDevice fail: " + std::system_category().message(hr)); }
			if (featureLevelInitialized != featureLevelRequested) { Logger::instance().logErr("D3D_FEATURE_LEVEL_11_0 fail: " + std::system_category().message(hr)); }

			hr = m_device.As(&m_device5);
			if (FAILED(hr)) { Logger::instance().logErr("Device Interface generation fail: " + std::system_category().message(hr)); }

			hr = m_devcon.As(&m_devcon4);
			if (FAILED(hr)) { Logger::instance().logErr("Device Context Interface generation fail: " + std::system_category().message(hr)); }

			hr = m_device.As(&m_devdebug);
			if (FAILED(hr)) { Logger::instance().logErr("Device debug generation fail: " + std::system_category().message(hr)); }


			// Write global pointers

			d3d::s_factory = m_factory5.Get();
			d3d::s_device = m_device5.Get();
			d3d::s_devcon = m_devcon4.Get();
		}
		void D3D::deinit()
		{
			// Pointers are released at destruction
			// Globals
			d3d::s_devcon = nullptr;
			d3d::s_device = nullptr;
			d3d::s_factory = nullptr;
			// ComPtrs
			m_factory.Reset();
			m_factory5.Reset();
			m_device.Reset();
			m_devcon.Reset();
			m_device5.Reset();
			m_devcon4.Reset();
			m_devdebug.Reset();
		}
	} // rend
} // engn