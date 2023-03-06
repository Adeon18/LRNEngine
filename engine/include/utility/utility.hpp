#pragma once
#include <iostream>
#include <iomanip>

#include <string>
#include <string.h>
#include <array>

#include <DirectXMath.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

namespace engn {
	using namespace DirectX;
	namespace util {
		//! Get the absolute path to the executable
		std::string getExePath();
		//! Get the absolute path to the executable(wstring)
		std::wstring getExePathW();
		//! Get the absolute path to the directory that contains the executable. PATH ends with "\"
		std::string getExeDir();
		//! Get the absolute path to the directory that contains the executable(wstring). PATH ends with "\"
		std::wstring getExeDirW();
		//! Get the directory from the specified path to file. PATH ends with "\"
		std::string getDirectoryFromPath(const std::string& path);
		//! Change the file extension from current to the specified in the second argument and return
		//! Extension should be specified as .ext
		std::string changeFileExt(const std::string& path, const std::string& ext);
		//! Convert the assimp library vector3 type to XMVECTOR
		XMVECTOR aiVector3DtoXMVECTOR(const aiVector3D& vec);
		//! Convert the assimp library aiMatrix4x4 type to XMMATRIX
		XMMATRIX aiMatrix4x4toXMMATRIX(const aiMatrix4x4& mat);
		//! Convert the assimp library vector3 type to XMFLOAT3
		XMFLOAT3 aiVector3DtoXMFLOAT3(const aiVector3D& vec);
		//! Test if all components of an XMVector are 0.0f - WARNING: is VERY SLOW!
		bool isXMVectorEmpty(const XMVECTOR& vec);
		//! Empty test for XMFloat4
		bool isXMVectorEmpty(const XMFLOAT4& vec);
		//! Align type to a specified size, return the aligned size value
		uint32_t alignUp(uint32_t typeSize, uint32_t alignTo);
		//! Convert string to wstring
		std::wstring stringToWstring(const std::string& str);
	} // util

	void XMVECTORtoStringArray(std::array<std::string, 4>& arr, const XMVECTOR& vec);

	// operators
	std::ostream& operator<<(std::ostream& os, const XMVECTOR& vec);
	std::ostream& operator<<(std::ostream& os, const XMMATRIX& mat);
} // engn