#pragma once

#include <deque>

#include "render/Graphics/EngineCamera.hpp"
#include "render/Systems/MeshSystem.hpp"

#include "source/caster/Caster.hpp"

namespace engn {
	namespace rm {
		class MeshRemover {
		public:
			struct DespawnEntry {
				// These insProps are in IncinerationGroup
				rend::InstanceProperties insProps;
				float timeSpawned;
			};

			static constexpr float DESPAWN_TIME = 2.0f;

			//! Remove the instance from Normal Group, add it to IncinerationGroup and start removal process
			void removeInstance(const cast::Caster::CollectedData& hitPointData, float currentTime);

			//! Update the removal process of each instance, remove them if nessesary
			void updateInstances(float currentTime);
		private:
			std::deque<DespawnEntry> m_despawningInstances;

			static constexpr std::array<XMVECTOR, 10> PARTICLE_COLORS{
				XMVECTOR{1.0f, 0.0f, 0.0f, 1.0f},
				XMVECTOR{0.0f, 1.0f, 0.0f, 1.0f},
				XMVECTOR{1.0f, 1.0f, 0.0f, 1.0f},
				XMVECTOR{0.0f, 0.0f, 1.0f, 1.0f},
				XMVECTOR{1.0f, 0.0f, 1.0f, 1.0f},
				XMVECTOR{0.0f, 1.0f, 1.0f, 1.0f},
				XMVECTOR{1.0f, 1.0f, 1.0f, 1.0f},
				XMVECTOR{1.0f, 0.5f, 0.5f, 1.0f},
				XMVECTOR{1.0f, 0.0f, 0.5f, 1.0f},
				XMVECTOR{1.0f, 0.5f, 0.0f, 1.0f}
			};
		};
	} // rm
} // engn