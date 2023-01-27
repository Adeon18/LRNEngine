#include <windows.h>

#pragma comment(lib, "Shlwapi.lib")
#include <Shlwapi.h>

#include "include/utility/utility.hpp"


namespace engn {
	namespace util {
		std::string getExePath() {
			char rawPath[MAX_PATH];
			GetModuleFileNameA(NULL, rawPath, MAX_PATH);
			return std::string{ rawPath };
		}
		std::wstring getExePathW() {
			wchar_t rawPath[MAX_PATH];
			GetModuleFileNameW(NULL, rawPath, MAX_PATH);
			return std::wstring{ rawPath };
		}

		std::string getExeDir() {
			std::string exePath = getExePath();
			char* exePathDyn = new char[exePath.length() + 1];
			strcpy_s(exePathDyn, exePath.length() + 1, exePath.c_str());
			// deprecated
			PathRemoveFileSpecA(exePathDyn);
			std::string dir = std::string(exePathDyn) + "\\";
			delete[] exePathDyn;
			return dir;
		}
		std::wstring getExeDirW() {
			std::wstring exePath = getExePathW();
			wchar_t* exePathDyn = new wchar_t[exePath.length() + 1];
			wcscpy_s(exePathDyn, exePath.length() + 1, exePath.c_str());
			// deprecated
			PathRemoveFileSpecW(exePathDyn);
			std::wstring dir = std::wstring(exePathDyn) + L"\\";
			delete[] exePathDyn;
			return dir;
		}

		XMVECTOR aiVector3DtoXMVECTOR(const aiVector3D& vec)
		{
			return { vec.x, vec.y, vec.z };
		}
		XMMATRIX aiMatrix4x4toXMMATRIX(const aiMatrix4x4& mat)
		{
			XMMATRIX outMat;
			/*for (uint32_t i = 0; i < 4; i++) {
				outMat.r[i] = { mat[i][0], mat[i][1], mat[i][2], mat[i][3]};
			}*/
			outMat.r[0] = { mat.a1, mat.a2, mat.a3, mat.a4 };
			outMat.r[1] = { mat.b1, mat.b2, mat.b3, mat.b4 };
			outMat.r[2] = { mat.c1, mat.c2, mat.c3, mat.c4 };
			outMat.r[3] = { mat.d1, mat.d2, mat.d3, mat.d4 };
			return outMat;
		}
		XMFLOAT3 aiVector3DtoXMFLOAT3(const aiVector3D& vec)
		{
			return { vec.x, vec.y, vec.z };
		}
	} // util

	void XMVECTORtoStringArray(std::array<std::string, 4>& arr, const XMVECTOR& vec) {
		arr[0] = std::to_string(XMVectorGetX(vec));
		arr[1] = std::to_string(XMVectorGetY(vec));
		arr[2] = std::to_string(XMVectorGetZ(vec));
		arr[3] = std::to_string(XMVectorGetW(vec));
	}

	std::ostream& operator<<(std::ostream& os, const XMVECTOR& vec)
	{
		std::array<std::string, 4> vecToStr;

		XMVECTORtoStringArray(vecToStr, vec);

		size_t maxLen = 0;
		for (const auto& el : vecToStr) {
			maxLen = (std::max)(maxLen, el.size());
		}

		os << "["
			<< std::setw(maxLen) << vecToStr[0] << " "
			<< std::setw(maxLen) << vecToStr[1] << " "
			<< std::setw(maxLen) << vecToStr[2] << " "
			<< std::setw(maxLen) << vecToStr[3] << "]";
		return os;
	}
	std::ostream& operator<<(std::ostream& os, const XMMATRIX& mat)
	{
		std::array<std::array<std::string, 4>, 4> matToStr;

		size_t maxLen = 0;
		for (size_t i = 0; i < 4; ++i) {
			XMVECTORtoStringArray(matToStr[i], mat.r[i]);
			for (const auto& el : matToStr[i]) {
				maxLen = (std::max)(maxLen, el.size());
			}
		}

		for (size_t i = 0; i < 4; ++i) {
			os << "[";
			for (size_t j = 0; j < 4; ++j) {
				os << std::setw(maxLen) << matToStr[i][j] << " ";
			}
			os << "]" << ((i == 3) ? "" : "\n");
		}
		return os;
	}
} // engn
