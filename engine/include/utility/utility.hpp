#pragma once
#include <iostream>
#include <iomanip>

#include <string>
#include <string.h>
#include <array>

#include <DirectXMath.h>

namespace engn {
	namespace util {
		//! Get the absolute path to the executable
		std::string getExePath();
		//! Get the absolute path to the executable(wstring)
		std::wstring getExePathW();
		//! Get the absolute path to the directory that contains the executable. PATH ends with "\"
		std::string getExeDir();
		//! Get the absolute path to the directory that contains the executable(wstring). PATH ends with "\"
		std::wstring getExeDirW();
	} // util

	using namespace DirectX;

	void XMVECTORtoStringArray(std::array<std::string, 4>& arr, const XMVECTOR& vec);

	// operators
	std::ostream& operator<<(std::ostream& os, const XMVECTOR& vec);
	std::ostream& operator<<(std::ostream& os, const XMMATRIX& mat);
} // engn