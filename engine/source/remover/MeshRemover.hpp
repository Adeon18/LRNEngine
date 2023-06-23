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
		};
	} // rm
} // engn