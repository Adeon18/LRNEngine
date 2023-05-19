#pragma once

#include <deque>

#include "render/Graphics/EngineCamera.hpp"
#include "render/Systems/MeshSystem.hpp"

namespace engn {
	namespace spwn {
		class MeshSpawner {
		public:
			//! Add the model with the respective instance data absed on camera position and direction
			void addDissolutionInstance(std::unique_ptr<rend::EngineCamera>& camPtr, float currentTime);
			//! Check if the specified time in dissolution group has passed and swap the respective instances
			void updateInstances(float currentTime);
		private:
			//! Here are instances that are still in the swapn animation
			std::deque<std::pair<float, rend::InstanceProperties>> m_spawningInstances;

#ifdef _WIN64
			const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\..\\";
#else
			const std::string TEX_REL_PATH_PREF = util::getExeDir() + "..\\";
#endif
			std::unordered_map<std::string, std::string> MODELS{
				{"HORSE", TEX_REL_PATH_PREF + "assets/Models/KnightHorse/KnightHorse.fbx"},
				{"SAMURAI", TEX_REL_PATH_PREF + "assets/Models/Samurai/Samurai.fbx"},
				{"TOWER", TEX_REL_PATH_PREF + "assets/Models/EastTower/EastTower.fbx"},
				{"CUBE", TEX_REL_PATH_PREF + "assets/Models/Cube/Cube.fbx"},
				{"SPHERE", TEX_REL_PATH_PREF + "assets/Models/Sphere/sphere.fbx"},
			};
		};
	} // swpn
} // engn
