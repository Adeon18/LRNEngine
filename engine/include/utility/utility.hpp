#pragma once

#include <string>
#include <string.h>

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
		//! Align type to a specified size, return the aligned size value
		uint32_t alignUp(uint32_t typeSize, uint32_t alignTo);
	} // util
} // engn