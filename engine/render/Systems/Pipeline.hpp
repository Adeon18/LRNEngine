#pragma once

#include "render/Graphics/DXShaders/VertexShader.hpp"
#include "render/Graphics/DXShaders/HullShader.hpp"
#include "render/Graphics/DXShaders/DomainShader.hpp"
#include "render/Graphics/DXShaders/GeometryShader.hpp"
#include "render/Graphics/DXShaders/PixelShader.hpp"

namespace engn {
	namespace rend {
		enum PipelineTypes {
			NORMAL_RENDER,
			HOLOGRAM_RENDER,
			FACE_NORMAL_DEBUG,
			WIREFRAME_DEBUG
		};

		struct Pipeline {
			D3D11_PRIMITIVE_TOPOLOGY topology;
			VertexShader vertexShader;
			HullShader hullShader;
			DomainShader domainShader;
			GeometryShader geometryShader;
			PixelShader pixelShader;
		};

		struct PipelineData {
			D3D11_INPUT_ELEMENT_DESC *layout;
			uint32_t layoutSize;
			D3D11_PRIMITIVE_TOPOLOGY topology;
			std::wstring VSpath;
			std::wstring HSpath;
			std::wstring DSpath;
			std::wstring GSpath;
			std::wstring PSpath;
		};

		inline void initPipeline(Pipeline& pipeline, const PipelineData& data) {
			pipeline.topology = data.topology;
			pipeline.vertexShader.init(data.VSpath, data.layout, data.layoutSize);
			pipeline.hullShader.init(data.HSpath);
			pipeline.domainShader.init(data.DSpath);
			pipeline.geometryShader.init(data.GSpath);
			pipeline.pixelShader.init(data.PSpath);
		}

		inline void bindPipeline(const Pipeline& pipeline) {
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