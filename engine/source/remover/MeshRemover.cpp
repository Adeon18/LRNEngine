#include "MeshRemover.hpp"

namespace engn {
	namespace rm {
		void MeshRemover::removeInstance(const cast::Caster::CollectedData& hitPointData, float currentTime)
		{
			// DO NOT DELETE LIGHTS LOL
			if (hitPointData.insProps.group == rend::GroupTypes::EMISSION_ONLY) { return; }

			auto& meshSystem = rend::MeshSystem::getInstance();
			std::shared_ptr<mdl::Model> mod = meshSystem.getModelByInsProps(hitPointData.insProps);
			rend::Material mt = meshSystem.getMaterialByInsProps(hitPointData.insProps);
			XMMATRIX mat = rend::TransformSystem::getInstance().getMatrixByIdCopy(meshSystem.getGroupMatrixIdx(hitPointData.insProps));
			XMVECTOR hitPosAndRadius = hitPointData.insHit.pos;
			hitPosAndRadius = XMVectorSetW(hitPosAndRadius, 1);

			meshSystem.removeNormalInstance(hitPointData.insProps);

			DespawnEntry dEntry{
				meshSystem.addIncinerationInstance(
					mod,
					{},
					{
						mat,
						{},
						hitPosAndRadius,
						{currentTime, currentTime, DESPAWN_TIME, DESPAWN_TIME}
					}
				).second,
				currentTime
			};

			m_despawningInstances.push_back(
				std::move(dEntry)
			);
		}
		void MeshRemover::updateInstances(float currentTime)
		{
			if (m_despawningInstances.size() == 0) { return; }
			auto entry = m_despawningInstances.front();
			if (currentTime - entry.timeSpawned > DESPAWN_TIME) {
				m_despawningInstances.pop_front();
				rend::MeshSystem::getInstance().removeIncinerationInstance(entry.insProps);

				//! Edit the indexes if the instance despawned, so other can too
				for (auto& el : m_despawningInstances) {
					if (el.insProps.modelIdx == entry.insProps.modelIdx) {
						if (el.insProps.materialIdx == entry.insProps.materialIdx) {
							if (el.insProps.instanceIdx > entry.insProps.instanceIdx) {
								--el.insProps.instanceIdx;
							}
						}
					}
				}
			}
		}
	} // rm
} // engn