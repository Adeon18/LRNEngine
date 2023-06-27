#pragma once

#include "render/Graphics/DXBuffers/StructuredBuffer.hpp"
#include "render/Graphics/DXBuffers/SimpleBuffer.hpp"

namespace engn {
	namespace rend {
		template<typename SBT, uint32_t SBN>
		class RingBuffer {
		public:
			void init() {
				m_particleData.init();
				m_rangeBuffer.init(true);
			}
			// Bind UAVs of buffers to some constant slots
			void bindToPipeline() {
				clearFromCS();

				ID3D11UnorderedAccessView* uavArr[2] = {
					m_particleData.getUAVPtr(),
					m_rangeBuffer.getUAVPtr()
				};

				d3d::s_devcon->OMSetRenderTargetsAndUnorderedAccessViews(
					D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
					nullptr,
					nullptr,
					5,
					2,
					uavArr,
					0
				);
			}

			void bindToCS() {
				clearFromPipeline();
				ID3D11UnorderedAccessView* uavArr[2] = {
					m_particleData.getUAVPtr(),
					m_rangeBuffer.getUAVPtr()
				};
				d3d::s_devcon->CSSetUnorderedAccessViews(1, 2, uavArr, 0);
			}
		private:
			void clearFromPipeline() {
				ID3D11UnorderedAccessView* uavArr[2] = {
					nullptr,
					nullptr
				};

				d3d::s_devcon->OMSetRenderTargetsAndUnorderedAccessViews(
					D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
					nullptr,
					nullptr,
					5,
					2,
					uavArr,
					0
				);
			}

			void clearFromCS() {
				ID3D11UnorderedAccessView* uavArr[2] = {
					nullptr,
					nullptr
				};
				d3d::s_devcon->CSSetUnorderedAccessViews(1, 2, uavArr, 0);
			}

			StructuredBuffer<SBT, SBN> m_particleData;
			SimpleBuffer<int32_t, 8> m_rangeBuffer;
		};
	} // rend
} // engn