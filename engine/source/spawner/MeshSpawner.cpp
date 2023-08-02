#include "MeshSpawner.hpp"

#include "render/UI/UI.hpp"

namespace engn {
	namespace spwn {
		void MeshSpawner::addDissolutionInstance(std::unique_ptr<rend::EngineCamera>& camPtr, float currentTime)
		{
			auto& spawnWidget = rend::UI::instance().getSpawnWidgetData();

			std::shared_ptr<mdl::Model> mptr = mdl::ModelManager::getInstance().getModel(
				MODELS[rend::UI::instance().getModelNameFromWidgetIdx(spawnWidget.modelToSpawnIdx)]
			);
			XMVECTOR translationVec = camPtr->getCamPosition() + spawnWidget.spawnDistance * camPtr->getCamForward();

			SpawnEntry entry{
				currentTime,
				spawnWidget.modelSpawnTime,
				rend::MeshSystem::getInstance().addDissolutionInstance(
					mptr,
					{},
					{ XMMatrixTranslationFromVector(translationVec), {}, {currentTime, currentTime, spawnWidget.modelSpawnTime, spawnWidget.modelSpawnTime} }
				).second,
				mptr,
				translationVec
			};

			m_spawningInstances.push_back(std::move(entry));
		}
		void MeshSpawner::updateInstances(float currentTime)
		{
			auto& spawnWidget = rend::UI::instance().getSpawnWidgetData();

			//! TODO: This has a problem, if an instance was spawned with 7 seconds spawn time and 
			//! then immediately after with 2 seconds swawn time, the second one will delete only after the first one.
			//! This can be fixed by either adding the for loop again, which is O(n^2) and increasing complexity of the code, 
			//! or use another data structure like std::map, hovewer erase() there is slover than pop_back.
			//! For I use O(n^2), this makes the deque basically useless, but I'll leave it here fir the future
			auto it = m_spawningInstances.begin();
			while (it != m_spawningInstances.end()) {
				auto firstInstance = *it;
				if (currentTime - firstInstance.timeSpawned > firstInstance.spawnTime) {
					rend::MeshSystem::getInstance().removeDissolutionInstance(firstInstance.instanceProperties);
					rend::MeshSystem::getInstance().addNormalInstance(
						firstInstance.mptr,
						{},
						{ XMMatrixTranslationFromVector(firstInstance.translationVec), {}, {} }
					);
					it = m_spawningInstances.erase(it);

					//! Edit the indexes if the instance despawned, so other can too
					for (auto& el : m_spawningInstances) {
						if (el.instanceProperties.modelIdx == firstInstance.instanceProperties.modelIdx) {
							if (el.instanceProperties.materialIdx == firstInstance.instanceProperties.materialIdx) {
								if (el.instanceProperties.instanceIdx > firstInstance.instanceProperties.instanceIdx) {
									--el.instanceProperties.instanceIdx;
								}
							}
						}
					}
				}
				else {
					++it;
				}
			}
		}
	} // spwn
} // engn