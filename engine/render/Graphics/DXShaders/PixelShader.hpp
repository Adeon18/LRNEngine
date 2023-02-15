#pragma once


#include "BaseShader.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A wrapper class for a PixelShader
		/// </summary>
		class PixelShader: public BaseShader {
		public:
			void init(const std::wstring& shaderPath);
			void bind() const;

			[[nodiscard]] ID3D11PixelShader* getShader() const { return m_shader.Get(); };
		private:
			//! Call create shader on d3d_device
			bool createShader(const std::wstring& shaderPath);

			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;
		};
	} // rend
} // engn