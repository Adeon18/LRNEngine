#pragma once

#include "BaseShader.hpp"

namespace engn {
	namespace rend {
		/// <summary>
		/// A wrapper for a compute shader
		/// </summary>
		class ComputeShader : public BaseShader {
		public:
			void init(const std::wstring& shaderPath);
			void bind() const;

			[[nodiscard]] ID3D11ComputeShader* getShader() const { return m_shader.Get(); };
		private:
			//! Call create shader on d3d_device
			bool createShader(const std::wstring& shaderPath);

			Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_shader;
		};
	} // rend
} // engn