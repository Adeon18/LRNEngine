#pragma once

#include "include/win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11_4.h>
#include <d3dcompiler.h>

#include "include/DxRes.hpp"

namespace engn
{
	namespace d3d {
		// global pointers to most used D3D11 objects for convenience:
		extern ID3D11Device5* s_device;
		extern ID3D11DeviceContext4* s_devcon;
		extern IDXGIFactory5* s_factory;
	} // d3d

	namespace rend {
		class D3D // a singletone for accessing global rendering resources
		{
		public:
			static D3D& getInstance()
			{
				static D3D instance;
				return instance;
			}
			void init();
			void deinit();

			D3D(const D3D&) = delete;
			void operator=(D3D const&) = delete;
		private:
			D3D() {}
		private:
			DxResPtr<IDXGIFactory> m_factory;
			DxResPtr<IDXGIFactory5> m_factory5;
			DxResPtr<ID3D11Device> m_device;
			DxResPtr<ID3D11Device5> m_device5;
			DxResPtr<ID3D11DeviceContext> m_devcon;
			DxResPtr<ID3D11DeviceContext4> m_devcon4;
			DxResPtr<ID3D11Debug> m_devdebug;
		};
	} // rend
} // engn

#include "include/win_undef.hpp"

