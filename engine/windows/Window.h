#pragma once

#include <iostream>

#include <windows.h>
#include <windowsx.h>


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

			//! Window Top Left position when it is firts generated
			static constexpr int WINDOW_TL_SCREEN_POS_X = 300;
			static constexpr int WINDOW_TL_SCREEN_POS_Y = 300;
		public:
			Window()
			{
				m_initWindowClass();

				// Register window class
				RegisterClassExW(&m_windowClassData.windowClass);

				// Gets the size of the actual window and stores it in the rect
				AdjustWindowRect(&m_windowRect, WS_OVERLAPPEDWINDOW, FALSE);

				m_createWindow();

				ShowWindow(m_windowClassData.handleWnd, SW_SHOW);
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

			// Bitmap information struct
			BITMAPINFO m_bitmapInfo;

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