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

			rend::MeshSystem::getInstance().addDissolutionInstance(
				mptr,
				{},
				{ XMMatrixTranslationFromVector(translationVec), {}, {1.0f, 0.0f, 1.0f, 0.0f} }
			);
		}
		void MeshSpawner::updateInstances(float currentTime)
		{
		}
	} // spwn
} // engn