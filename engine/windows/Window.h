﻿#pragma once

#include <iostream>

#include <windows.h>
#include <windowsx.h>

#include <d3d11_4.h>

#include "utils/Logger/Logger.hpp"

#include "render/D3D/d3d.hpp"

#include "render/Graphics/DXRTVs/LDRRenderTarget.hpp"
#include "render/Graphics/DXRTVs/HDRRenderTarget.hpp"

#include "utils/paralell_executor/parallel_executor.h"


namespace engn {
	namespace win {
		//! A basic window struct data
		struct WindowClassData {
			// Window handler
			HWND handleWnd;
			// Handle to the device context
			HDC handleDC;
			HINSTANCE hInstance;
			// Struct with info for the window class
			WNDCLASSEXW windowClass;
		};

		//! A wrapper for the data that gets passed to a renderer
		struct WindowRenderData
		{
			void* screenBuffer = nullptr;
			int screenWidth{};
			int screenHeight{};
		};

		template<int W, int H>
		class Window {
		public:
			inline static const wchar_t* WINDOW_NAME = L"EngineClass";
			const wchar_t* WINDOW_TITLE = L"Engine";

			//! Window Top Left position when it is firts generated
			static constexpr int WINDOW_TL_SCREEN_POS_X = 300;
			static constexpr int WINDOW_TL_SCREEN_POS_Y = 300;
		public:
			Window()
			{
				m_initWindowClass();

				// Register window class
				RegisterClassExW(&m_windowClassData.windowClass);

				// Adjust rect
				m_initWindowRect();
				
				m_createWindow();

				ShowWindow(m_windowClassData.handleWnd, SW_SHOW);

				// Initialize DX stuff for render
				initSwapchain();

				initBackBuffer();
				initRenderTargetViews();
				initDepthStensilBuffer();
				initViewPort();
				bindViewport();
			}


			~Window() {
				UnregisterClassW(WINDOW_NAME, m_windowClassData.hInstance);
				DestroyWindow(m_windowClassData.handleWnd);
			}


			//! Callback message handler
			static LRESULT CALLBACK handleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message) {
				case WM_DESTROY:
				{
					m_destroyWindow();
					return 0;
				} break;
				case WM_SIZE:
				{
					m_updateWindowSize(hWnd);
					return 0;
				} break;
				}
				// Handle what the switch didn't
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			//! Fill the swapchain description and initialize it
			void initSwapchain()
			{
				DXGI_SWAP_CHAIN_DESC1 desc;

				// clear out the struct for use
				memset(&desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));

				// fill the swap chain description struct
				// If width and height are unspecified, they will be pulled from the output window
				desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
				desc.BufferCount = 2;
				desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT; // CPU access options for back buffer
				desc.Flags = 0;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;                               // how many multisamples
				desc.SampleDesc.Quality = 0;                             // ???
				desc.Scaling = DXGI_SCALING_NONE; // Identifies scaling behaviour if the size of the back buffer is not equal to the output target
				desc.Stereo = false;
				desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


				HRESULT hr = d3d::s_factory->CreateSwapChainForHwnd(
					d3d::s_device,
					m_windowClassData.handleWnd,
					&desc,
					nullptr, nullptr,
					m_swapChain.GetAddressOf()
				);

				if (FAILED(hr)) {
					m_logger.logErr("Window CreateSwapChainForHwnd fail: " + std::system_category().message(hr));
					return;
				}
			}

			//! Called at resize. Free the LDR backbuffer and HDR RTV and resize it to a new one
			void initBackBuffer() {
				m_renderTargetHDR.releaseAll();
				m_renderTargetLDRFinal.releaseAll();

				m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

				HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_renderTargetLDRFinal.getTexturePtrAddress()));
				if (FAILED(hr)) {
					m_logger.logErr("Window GetBuffer on BackBuffer fail: " + std::system_category().message(hr));
					return;
				}
			}

			//! Called at resize AFTER initBackBuffer.
			//! Initialize the renderTargetViews(HDR and LDR)
			void initRenderTargetViews()
			{
				m_renderTargetHDR.init(m_windowRenderData.screenWidth, m_windowRenderData.screenHeight);
				m_renderTargetLDRFinal.init();
			}
			//! Bind the initial HDR rtv
			void bindInitialRTV() {
				m_renderTargetHDR.OMSetCurrent(m_depthStensilView.Get());
			}

			//! Bind the backBuffer
			void bindBackBuffer() {
				// TODO: Change to nullptr
				m_renderTargetLDRFinal.OMSetCurrent(m_depthStensilView.Get());
			}

			//! Initialize the Depth Stencil Buffer and View, buffers are freed at every resize
			void initDepthStensilBuffer() {
				m_depthStensilView.Reset();
				m_depthStensilBuffer.Reset();

				D3D11_TEXTURE2D_DESC depthStencilDesc{};
				depthStencilDesc.Width = m_windowRenderData.screenWidth;
				depthStencilDesc.Height = m_windowRenderData.screenHeight;
				depthStencilDesc.MipLevels = 1;
				depthStencilDesc.ArraySize = 1;
				depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilDesc.SampleDesc.Count = 1;
				depthStencilDesc.SampleDesc.Quality = 0;
				depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
				depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depthStencilDesc.CPUAccessFlags = 0;
				depthStencilDesc.MiscFlags = 0;

				HRESULT hr = d3d::s_device->CreateTexture2D(&depthStencilDesc, NULL, m_depthStensilBuffer.GetAddressOf());
				if (FAILED(hr)) {
					m_logger.logErr("Window DepthStensilBuffer create fail: " + std::system_category().message(hr));
					return;
				}

				hr = d3d::s_device->CreateDepthStencilView(m_depthStensilBuffer.Get(), NULL, m_depthStensilView.GetAddressOf());
				if (FAILED(hr)) {
					m_logger.logErr("Window CreateDepthStencilView fail: " + std::system_category().message(hr));
					return;
				}
			}

			//! Called at resize AFTER initRenderTargetView. Initialized the viewport with new screen parameters
			void initViewPort() {
				m_viewPort.TopLeftX = m_windowRect.left;
				m_viewPort.TopLeftY = m_windowRect.top;
				m_viewPort.Width = m_windowRect.right - m_windowRect.left;
				m_viewPort.Height = m_windowRect.bottom - m_windowRect.top;
				// It is set this way, despite the reversed depth matrix
				m_viewPort.MinDepth = 0.0f;
				m_viewPort.MaxDepth = 1.0f;
			}

			//! Bind the viwport to the rasterizer
			void bindViewport() { d3d::s_devcon->RSSetViewports(1, &m_viewPort); }

			//! Poll window for resize, MUST BE CALLED at the start of every frame to support resizing
			bool pollResize() {
				bool wasResized = false;
				// Resize if there was a call
				if (m_toBeResized)
				{
					initBackBuffer();
					initRenderTargetViews();
					initDepthStensilBuffer();
					initViewPort();
					m_toBeResized = false;
					wasResized = true;
				}
				return true;
			}

			//! Clears the RTV that we write to initially(HDR in out case), clear the DSV too
			void bindAndClearInitialRTV(float* color)
			{
				// So we don't crash at window minimize
				if (m_windowRenderData.screenWidth == 0 || m_windowRenderData.screenHeight == 0) { return; }
				// We set the rendertargetview each frame
				bindInitialRTV();
				bindViewport();
				m_renderTargetHDR.clear(color);
				// Depth is 0.0f because we utilize reversed depth matrix
				d3d::s_devcon->ClearDepthStencilView(m_depthStensilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
			}

			//! Clear the final LDR RTV and bind it
			void bindAndClearBackbuffer(float* color) {
				// So we don't crash at window minimize
				if (m_windowRenderData.screenWidth == 0 || m_windowRenderData.screenHeight == 0) { return; }
				bindBackBuffer();
				bindViewport();
				m_renderTargetLDRFinal.clear(color);
				// TODO: Remove depth stencil
				d3d::s_devcon->ClearDepthStencilView(m_depthStensilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
			}

			//! Present the swapchain. Called after clear and Engine::render
			void present() {
				m_swapChain->Present(0, NULL);
			}

			[[nodiscard]] HWND getHandler() const { return m_windowClassData.handleWnd; }

			// TODO: Copy of struct
			[[nodiscard]] RECT getWindowRect() const { return m_windowRect; }

			//! Get all the availible window data
			[[nodiscard]] WindowRenderData& getWindowData() const { return m_windowRenderData; };

			[[nodiscard]] void* getBitmapBuffer() const { return m_windowRenderData.screenBuffer; }

			[[nodiscard]] int getWidth() const { return m_windowRenderData.screenWidth; }
			[[nodiscard]] int getHeight() const { return m_windowRenderData.screenHeight; }

			[[nodiscard]] int getBIWidth() const { return m_windowRenderData.bufferWidth; }
			[[nodiscard]] int getBIHeight() const { return m_windowRenderData.bufferHeight; }

			[[nodiscard]] rend::HDRRenderTarget& getHDRRTVRef() { return m_renderTargetHDR; }
			[[nodiscard]] rend::LDRRenderTarget& getLDRRTVRef() { return m_renderTargetLDRFinal; }


		private:
			Logger& m_logger = Logger::instance();

			// Basic window data that get's passed for renreding
			inline static WindowRenderData m_windowRenderData{
				nullptr,
				W, H,
			};

			inline static RECT m_windowRect{ 0, 0, W, H };

			inline static bool m_toBeResized = true;

			inline static WindowClassData m_windowClassData;

			rend::LDRRenderTarget m_renderTargetLDRFinal;
			rend::HDRRenderTarget m_renderTargetHDR;

			D3D11_VIEWPORT m_viewPort;

			Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStensilView;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStensilBuffer;

			// Bitmap information struct
			BITMAPINFO m_bitmapInfo;

		private:
			//! Initialize window class and basic window data
			void m_initWindowClass() {
				m_windowClassData.hInstance = GetModuleHandle(nullptr);
				// Null out the struct - need to make sure there are zeros
				ZeroMemory(&m_windowClassData.windowClass, sizeof(WNDCLASSEXW));

				//�Fill�in�the�struct�with�the�needed�information
				m_windowClassData.windowClass.cbSize = sizeof(WNDCLASSEXW);					// struct size
				m_windowClassData.windowClass.style = CS_HREDRAW | CS_VREDRAW;				// 
				m_windowClassData.windowClass.lpfnWndProc = Window::handleMessages;			// handler function
				m_windowClassData.windowClass.hInstance = m_windowClassData.hInstance;						// application copy
				m_windowClassData.windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);	// load the cursor
				m_windowClassData.windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;			// define the brush that will color window
				m_windowClassData.windowClass.lpszClassName = WINDOW_NAME;					// Name of the class, L because 16 bit Unicode
			}
			
			//! Sets the window location and gets the actual client size
			void m_initWindowRect() {
				m_windowRect.left = 300;
				m_windowRect.top = 300;
				m_windowRect.right = m_windowRect.left + m_windowRenderData.screenWidth;
				m_windowRect.bottom = m_windowRect.top + m_windowRenderData.screenHeight;
				AdjustWindowRect(&m_windowRect, WS_OVERLAPPEDWINDOW, FALSE);
			}

			//! Create the window and initialize additional window class data
			void m_createWindow() {
				m_windowClassData.handleWnd = CreateWindowExW(
					NULL,
					WINDOW_NAME,						//�name�of�the�window�class
					WINDOW_TITLE,						//�title�of�the�window
					WS_OVERLAPPEDWINDOW,				//�window�style
					WINDOW_TL_SCREEN_POS_X,				//�x-position�of�the�window
					WINDOW_TL_SCREEN_POS_Y,				//�y-position�of�the�window
					m_windowRect.right - m_windowRect.left,	//�width�of�the�window
					m_windowRect.bottom - m_windowRect.top, //�height�of�the�window
					nullptr,							//�we�have�no�parent�window,�NULL
					nullptr,							//�we�aren't�using�menus,�NULL
					m_windowClassData.hInstance,						//�application�handle
					nullptr);					//�used�with�multiple�windows,�NULL

				m_windowClassData.handleDC = GetDC(m_windowClassData.handleWnd);
			}

			//! Destroy window and quit
			static void m_destroyWindow()
			{
				PostQuitMessage(0);
			}

			//! Update the size of the window at resize
			static void m_updateWindowSize(HWND hWnd)
			{
				RECT newClientRect;
				GetClientRect(hWnd, &newClientRect);

				//// If window is minimized - the results are 0 => handled in during render()

				m_windowRenderData.screenWidth = newClientRect.right - newClientRect.left;
				m_windowRenderData.screenHeight = newClientRect.bottom - newClientRect.top;

				m_windowRect = newClientRect;
				AdjustWindowRect(&m_windowRect, WS_OVERLAPPEDWINDOW, FALSE);

				m_toBeResized = true;
			}
		};
	} // win
} // engn