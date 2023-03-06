#include <windows.h>

#pragma comment(lib, "Shlwapi.lib")
#include <Shlwapi.h>

#include <filesystem>

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

		std::string getDirectoryFromPath(const std::string& path)
		{
			return std::filesystem::path{ path }.parent_path().string() + "\\";
		}
		
		std::string changeFileExt(const std::string& path, const std::string& ext)
		{
			return path.substr(0, path.find_last_of('.')) + ext;
		}

		XMVECTOR aiVector3DtoXMVECTOR(const aiVector3D& vec)
		{
			return { vec.x, vec.y, vec.z };
		}
		XMMATRIX aiMatrix4x4toXMMATRIX(const aiMatrix4x4& mat)
		{
			XMMATRIX outMat;
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
		bool isXMVectorEmpty(const XMVECTOR& vec)
		{
			return XMVectorGetX(vec) == 0.0f && XMVectorGetY(vec) == 0.0f && XMVectorGetZ(vec) == 0.0f && XMVectorGetW(vec) == 0.0f;
		}
		bool isXMVectorEmpty(const XMFLOAT4& vec)
		{
			return vec.x == 0.0f && vec.y == 0.0f && vec.z == 0.0f && vec.w == 0.0f;
		}
		uint32_t util::alignUp(uint32_t typeSize, uint32_t alignTo)
		{
			return (typeSize + 16) - (typeSize % 16);
		}
		std::wstring util::stringToWstring(const std::string& str)
		{
			return std::wstring(str.begin(), str.end());
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
