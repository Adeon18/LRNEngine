#pragma once

#include <iostream>

#include <windows.h>
#include <windowsx.h>


struct WindowClassData {

};


struct WindowRenderData
{
	void* screenBuffer = nullptr;
	int screenWidth{};
	int screenHeight{};
	int bufferWidth{};
	int bufferHeight{};
};

template<int W, int H>
class Window {
public:
	const wchar_t* WINDOW_NAME = L"EngineClass";
	const wchar_t* WINDOW_TITLE = L"Engine";
public:
	Window() :
		m_HInstance{ GetModuleHandle(nullptr) }
	{

		// Null out the struct - need to make sure there are zeros
		ZeroMemory(&m_WindowClass, sizeof(WNDCLASSEXW));

		// Fill in the struct with the needed information
		m_WindowClass.cbSize = sizeof(WNDCLASSEXW);					// struct size
		m_WindowClass.style = CS_HREDRAW | CS_VREDRAW;				// 
		m_WindowClass.lpfnWndProc = Window::handleMessages;			// handler function
		m_WindowClass.hInstance = m_HInstance;						// application copy
		m_WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);	// load the cursor
		m_WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;			// define the brush that will color window
		m_WindowClass.lpszClassName = WINDOW_NAME;					// Name of the class, L because 16 bit Unicode

		// Register window class
		RegisterClassExW(&m_WindowClass);

		// Gets the size of the actual window and stores it in the rect
		AdjustWindowRect(&m_windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window and use result as handle
		m_handleWnd = CreateWindowExW(
			NULL,
			WINDOW_NAME,						// name of the window class
			WINDOW_TITLE,						// title of the window
			WS_OVERLAPPEDWINDOW,				// window style
			300,								// x-position of the window
			300,								// y-position of the window
			m_windowRect.right - m_windowRect.left,	// width of the window
			m_windowRect.bottom - m_windowRect.top, // height of the window
			nullptr,							// we have no parent window, NULL
			nullptr,							// we aren't using menus, NULL
			m_HInstance,						// application handle
			nullptr);					// used with multiple windows, NULL

		m_handleDC = GetDC(m_handleWnd);

		ShowWindow(m_handleWnd, SW_SHOW);

		//std::cout << SetWindowLongPtrW(m_handleWnd, 16, reinterpret_cast<LONG_PTR>(this)) << std::endl;
	}


	~Window() { UnregisterClassW(WINDOW_NAME, m_HInstance); }


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


	//! Allocate memory for the bitmap that gets drawn on screen, availible via getBitmapBuffer
	void allocateBitmapBuffer()
	{
		// Free old memory if screen was resized
		if (m_windowData.screenBuffer && m_toBeResized)
		{
			VirtualFree(m_windowData.screenBuffer, 0, MEM_RELEASE);
			m_windowData.screenBuffer = nullptr;
		}

		// Fill in the bitmapinfo
		m_bitmapInfo.bmiHeader.biSize = sizeof(m_bitmapInfo.bmiHeader);	// Why?....
		m_bitmapInfo.bmiHeader.biWidth = m_windowData.bufferWidth;
		m_bitmapInfo.bmiHeader.biHeight = -m_windowData.bufferHeight;					// Top down render
		m_bitmapInfo.bmiHeader.biPlanes = 1;							// Legacy
		m_bitmapInfo.bmiHeader.biBitCount = sizeof(COLORREF) * 8;		// We allocate for COLORREF
		m_bitmapInfo.bmiHeader.biCompression = BI_RGB;					// If you need compression, we don't because RGB
		m_bitmapInfo.bmiHeader.biSizeImage = 0;							// Unused if no compression
		m_bitmapInfo.bmiHeader.biXPelsPerMeter = 0;						// We don't need to know about pixels per meter
		m_bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
		m_bitmapInfo.bmiHeader.biClrUsed = 0;							// Last 2 are for pallets
		m_bitmapInfo.bmiHeader.biClrImportant = 0;

		// Allocate memory for bits - reallocate if screnn was resized
		const size_t bitmapMemorySize = m_windowData.bufferWidth * m_windowData.bufferHeight * 4;
		if (!m_windowData.screenBuffer && m_toBeResized)
		{
			m_windowData.screenBuffer = VirtualAlloc(nullptr, bitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			m_toBeResized = false;
		}
	}


	//! Calls stretchDIBits which in turn copies all data from the bitmap buffer to screen
	void flush() const
	{
		StretchDIBits(
			m_handleDC,						// Device context handle
			0, 0,
			m_windowData.screenWidth, m_windowData.screenHeight,
			0, 0, m_windowData.bufferWidth, m_windowData.bufferHeight,
			m_windowData.screenBuffer,
			&m_bitmapInfo,
			DIB_RGB_COLORS,					// The color are RGB, can also be a pallet of you need to have limited colors
			SRCCOPY							// We just want to copy to buffer and not do any operations to it
		);
	}

	[[nodiscard]] HWND getHandler() const { return m_handleWnd; }
	// TODO: Copy of struct - does not word after resize!
	[[nodiscard]] RECT getWindowRect() const { return m_windowRect; }
	//! Get all the availible window data
	[[nodiscard]] WindowRenderData getWindowData() const { return m_windowData; };

	[[nodiscard]] void* getBitmapBuffer() const { return m_windowData.screenBuffer; }

	[[nodiscard]] int getWidth() const { return m_windowData.screenWidth; }
	[[nodiscard]] int getHeight() const { return m_windowData.screenHeight; }

	[[nodiscard]] int getBIWidth() const { return m_windowData.bufferWidth; }
	[[nodiscard]] int getBIHeight() const { return m_windowData.bufferHeight; }


private:
	// Basic window data that get's passed for renreding
	inline static WindowRenderData m_windowData{
		nullptr,
		W, H,
		W / 2, H / 2
	};

	inline static RECT m_windowRect{ 0, 0, W, H };

	inline static bool m_toBeResized = true;


	// Window handler
	HWND m_handleWnd;
	// Handle to the device context
	HDC m_handleDC;
	HINSTANCE m_HInstance;
	// The rect that represents the size of the actual window(not the part where you can draw)
	// Struct with info for the window class
	WNDCLASSEXW m_WindowClass;

	// Bitmap information struct
	BITMAPINFO m_bitmapInfo;

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

		m_windowData.screenWidth = newClientRect.right - newClientRect.left;
		m_windowData.screenHeight = newClientRect.bottom - newClientRect.top;

		m_windowData.bufferWidth = m_windowData.screenWidth / 2;
		m_windowData.bufferHeight = m_windowData.screenHeight / 2;

		m_windowRect = newClientRect;
		AdjustWindowRect(&m_windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		m_toBeResized = true;
	}

};