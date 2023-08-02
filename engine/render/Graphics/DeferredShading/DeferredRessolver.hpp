#pragma once

#include "GBuffer.hpp"
#include "render/Systems/Pipeline.hpp"
#include "render/Graphics/DXRTVs/BindableRenderTarget.hpp"
#include "render/Graphics/DXBuffers/ConstantBuffer.hpp"

namespace engn {
	namespace rend {
		class DeferredRessolver {
		public:
			void init();

			void ressolve(const GBuffer& gBuf);
		private:
			void initPipelines();

			Pipeline m_pbrPipeline;
			Pipeline m_emissionPipeline;
		};
	}
} // engn