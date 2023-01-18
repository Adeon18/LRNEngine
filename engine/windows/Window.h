#pragma once

#include <iostream>

#include <windows.h>
#include <windowsx.h>

#include <d3d11_4.h>

//#include "include/DxRes.hpp"
#include "render/D3D/d3d.hpp"
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
			int bufferWidth{};
			int bufferHeight{};
		};

		template<int W, int H, int BDS>
		class Window {
		public:
			const wchar_t* WINDOW_NAME = L"EngineClass";
			const wchar_t* WINDOW_TITLE = L"Engine";
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
				initRenderTargetView();
				initViewPort();
			}


			~Window() { UnregisterClassW(WINDOW_NAME, m_windowClassData.hInstance); }


			//! Callback message handler
			static LRESULT CALLBACK handleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message) {
				case WM_DESTROY:
				{
					m_destroyWindow();
				} break;
				case WM_SIZE:
				{
					m_updateWindowSize(hWnd);
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


				HRESULT res = d3d::s_factory->CreateSwapChainForHwnd(
					d3d::s_device,
					m_windowClassData.handleWnd,
					&desc,
					nullptr, nullptr,
					m_swapChain.reset()
				);

				if (FAILED(res)) { std::cout << "CreateSwapChainForHwnd fail" << std::endl; }
			}

			//! Called at resize. Free the backBuffer and resize it to a new one
			void initBackBuffer() {
				// Release the RTV before resizing
				if (m_renderTargetView.valid())
				{
					m_renderTargetView.release();
				}
				// Release the backBuffer before resize
				if (m_backBuffer.valid())
				{
					m_backBuffer.release();
					m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
				}

				HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_backBuffer.reset());
				if (FAILED(result)) { std::cout << "onResize GetBuffer fail" << std::endl; }

				ID3D11Texture2D* pTextureInterface = 0;
				m_backBuffer->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
				pTextureInterface->GetDesc(&m_backBufferDesc);
				pTextureInterface->Release();
			}

			//! Called at resize AFTER initBackBuffer. Initialize the renderTargetView and set it as a Render target to device context
			void initRenderTargetView()
			{
				HRESULT res = d3d::s_device->CreateRenderTargetView(m_backBuffer.ptr(), NULL, m_renderTargetView.reset());
				if (FAILED(res)) { std::cout << "onResize CreateRenderTargetView fail" << std::endl; }
				setRenderTargetView();
			}

			void setRenderTargetView() {
				d3d::s_devcon->OMSetRenderTargets(1, m_renderTargetView.getAddressOf(), NULL);
			}

			//! Called at resize AFTER initRenderTargetView. Initialized the viewport with new screen parameters
			void initViewPort() {
				D3D11_VIEWPORT viewPort;
				memset(&viewPort, 0, sizeof(D3D11_VIEWPORT));
					
				viewPort.TopLeftX = m_windowRect.left;
				viewPort.TopLeftY = m_windowRect.top;
				viewPort.Width = m_windowRect.right - m_windowRect.left;
				viewPort.Height = m_windowRect.bottom - m_windowRect.top;

				d3d::s_devcon->RSSetViewports(1, &viewPort);
			} 

			//! Set the RTV and clear the window with the specified color
			void clear(float* color)
			{
				// Resize if there was a call
				if (m_toBeResized)
				{
					initBackBuffer();
					initRenderTargetView();
					initViewPort();
					m_toBeResized = false;
				}
				// We set the rendertargetview each frame
				setRenderTargetView();
				d3d::s_devcon->ClearRenderTargetView(m_renderTargetView.ptr(), color);
			}
			//! Present the swapchain. Called after clear and Engine::render
			void present() {
				m_swapChain->Present(0, NULL);
			}

			//! Allocate memory for the bitmap that gets drawn on screen, availible via getBitmapBuffer, return true if allocation happened
			bool allocateBitmapBuffer()
			{
				// Free old memory if screen was resized
				if (m_windowRenderData.screenBuffer && m_toBeResized)
				{
					VirtualFree(m_windowRenderData.screenBuffer, 0, MEM_RELEASE);
					m_windowRenderData.screenBuffer = nullptr;
				}

				// Allocate memory for bits - reallocate if screnn was resized
				if (!m_windowRenderData.screenBuffer && m_toBeResized)
				{
					const size_t bitmapMemorySize = m_windowRenderData.bufferWidth * m_windowRenderData.bufferHeight * 4;
					m_fillBitmapInfo();

					m_windowRenderData.screenBuffer = VirtualAlloc(nullptr, bitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					m_toBeResized = false;
					return true;
				}
				return false;
			}

			//! Calls stretchDIBits which in turn copies all data from the bitmap buffer to screen
			void flush() const
			{
				StretchDIBits(
					m_windowClassData.handleDC,
					0, 0,
					m_windowRenderData.screenWidth, m_windowRenderData.screenHeight,
					0, 0,
					m_windowRenderData.bufferWidth, m_windowRenderData.bufferHeight,
					m_windowRenderData.screenBuffer,
					&m_bitmapInfo,
					DIB_RGB_COLORS,		// The color are RGB, can also be a pallet of you need to have limited colors
					SRCCOPY				// We just want to copy to buffer and not do any operations to it
				);
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


		private:
			// Basic window data that get's passed for renreding
			inline static WindowRenderData m_windowRenderData{
				nullptr,
				W, H,
				W / BDS, H / BDS
			};

			inline static RECT m_windowRect{ 0, 0, W, H };

			inline static bool m_toBeResized = true;

			WindowClassData m_windowClassData;

			DxResPtr<IDXGISwapChain1> m_swapChain;
			DxResPtr<ID3D11Texture2D> m_backBuffer;
			DxResPtr<ID3D11RenderTargetView> m_renderTargetView;
			D3D11_TEXTURE2D_DESC m_backBufferDesc;

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
					m_windowRect.left,								//�x-position�of�the�window
					m_windowRect.top,								//�y-position�of�the�window
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

				m_windowRenderData.bufferWidth = m_windowRenderData.screenWidth / BDS;
				m_windowRenderData.bufferHeight = m_windowRenderData.screenHeight / BDS;

				m_windowRect = newClientRect;
				AdjustWindowRect(&m_windowRect, WS_OVERLAPPEDWINDOW, FALSE);

				m_toBeResized = true;
			}

			//! Fill in the bitmapinfo at screen buffer allocation
			void m_fillBitmapInfo() {
				m_bitmapInfo.bmiHeader.biSize = sizeof(m_bitmapInfo.bmiHeader);	// Why?....
				m_bitmapInfo.bmiHeader.biWidth = m_windowRenderData.bufferWidth;
				m_bitmapInfo.bmiHeader.biHeight = m_windowRenderData.bufferHeight;	// Top down render
				m_bitmapInfo.bmiHeader.biPlanes = 1;							// Legacy
				m_bitmapInfo.bmiHeader.biBitCount = sizeof(COLORREF) * 8;		// We allocate for COLORREF
				m_bitmapInfo.bmiHeader.biCompression = BI_RGB;					// If you need compression, we don't because RGB
				m_bitmapInfo.bmiHeader.biSizeImage = 0;							// Unused if no compression
				m_bitmapInfo.bmiHeader.biXPelsPerMeter = 0;						// We don't need to know about pixels per meter
				m_bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
				m_bitmapInfo.bmiHeader.biClrUsed = 0;							// Last 2 are for pallets
				m_bitmapInfo.bmiHeader.biClrImportant = 0;
			}
		};
	} // win
} // engn