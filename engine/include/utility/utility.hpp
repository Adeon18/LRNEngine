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
		//! Convert the assimp library vector3 type to XMVECTOR
		XMVECTOR aiVector3DtoXMVECTOR(const aiVector3D& vec);
		XMMATRIX aiMatrix4x4toXMMATRIX(const aiMatrix4x4& mat);
		XMFLOAT3 aiVector3DtoXMFLOAT3(const aiVector3D& vec);
	} // util

	void XMVECTORtoStringArray(std::array<std::string, 4>& arr, const XMVECTOR& vec);

	// operators
	std::ostream& operator<<(std::ostream& os, const XMVECTOR& vec);
	std::ostream& operator<<(std::ostream& os, const XMMATRIX& mat);
} // engn