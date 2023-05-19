#pragma once

#include "render/Graphics/DXShaders/VertexShader.hpp"
#include "render/Graphics/DXShaders/HullShader.hpp"
#include "render/Graphics/DXShaders/DomainShader.hpp"
#include "render/Graphics/DXShaders/GeometryShader.hpp"
#include "render/Graphics/DXShaders/PixelShader.hpp"

namespace engn {
	namespace rend {
		//! The enum that defines all the types of pipelines with shaders - basically a search key for a pipeline
		enum PipelineTypes {
			NORMAL_RENDER,
			DISSOLUTION_RENDER,
			HOLOGRAM_RENDER,
			EMISSION_ONLY_RENDER,
			FACE_NORMAL_DEBUG,
			WIREFRAME_DEBUG
		};
		//! A definition of a pipeline, binding/rebinding this struct means changing the pipeline
		struct Pipeline {
			D3D11_PRIMITIVE_TOPOLOGY topology;
			VertexShader vertexShader;
			HullShader hullShader;
			DomainShader domainShader;
			GeometryShader geometryShader;
			PixelShader pixelShader;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStensilState;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
			Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
		};
		//! The data from which the pipeline is initialized
		struct PipelineData {
			D3D11_INPUT_ELEMENT_DESC *layout;
			uint32_t layoutSize;
			D3D11_PRIMITIVE_TOPOLOGY topology;
			std::wstring VSpath;
			std::wstring HSpath;
			std::wstring DSpath;
			std::wstring GSpath;
			std::wstring PSpath;
			D3D11_RASTERIZER_DESC rasterizerDesc;
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
		};
		//! Initialize the Pipeline struct from the pipeline data
		inline void initPipeline(Pipeline& pipeline, const PipelineData& data) {
			pipeline.topology = data.topology;
			pipeline.vertexShader.init(data.VSpath, data.layout, data.layoutSize);
			pipeline.hullShader.init(data.HSpath);
			pipeline.domainShader.init(data.DSpath);
			pipeline.geometryShader.init(data.GSpath);
			pipeline.pixelShader.init(data.PSpath);

			HRESULT hr = d3d::s_device->CreateRasterizerState(&data.rasterizerDesc, pipeline.rasterizerState.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("Pipeline::CreateRasterizerState fail: " + std::system_category().message(hr));
				return;
			}

			hr = d3d::s_device->CreateDepthStencilState(&data.depthStencilDesc, pipeline.depthStensilState.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("Pipeline::CreateDepthStencilState fail: " + std::system_category().message(hr));
				return;
			}

			D3D11_BLEND_DESC blendDesc{};
			blendDesc.RenderTarget[0] = data.blendDesc;
			
			hr = d3d::s_device->CreateBlendState(&blendDesc, pipeline.blendState.GetAddressOf());
			if (FAILED(hr)) {
				Logger::instance().logErr("Pipeline::CreateBlendState fail: " + std::system_category().message(hr));
				return;
			}

		}
		//! Bind the respective pipeline
		inline void bindPipeline(const Pipeline& pipeline) {
			d3d::s_devcon->RSSetState(pipeline.rasterizerState.Get());
			d3d::s_devcon->OMSetDepthStencilState(pipeline.depthStensilState.Get(), 0);
			d3d::s_devcon->OMSetBlendState(pipeline.blendState.Get(), NULL, 0xFFFFFFFF);
			d3d::s_devcon->IASetInputLayout(pipeline.vertexShader.getInputLayout());
			d3d::s_devcon->IASetPrimitiveTopology(pipeline.topology);
			pipeline.vertexShader.bind();
			pipeline.hullShader.bind();
			pipeline.domainShader.bind();
			pipeline.geometryShader.bind();
			pipeline.pixelShader.bind();
		}
	} // rend
} // engn