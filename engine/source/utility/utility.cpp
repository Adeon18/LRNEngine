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
		uint32_t alignUp(uint32_t typeSize, uint32_t alignTo)
		{
			return (typeSize + 16) - (typeSize % 16);
		}
	} // util
} // engn