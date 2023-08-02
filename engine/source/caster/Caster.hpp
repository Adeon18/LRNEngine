#pragma once

#include "render/Systems/MeshSystem.hpp"

#include "render/Graphics/EngineCamera.hpp"

namespace engn {
	namespace cast {

		//! A simple object that just casts the ray to whenever the camera is pointing and returns the intersection Data
		class Caster {
		public:
			struct CollectedData {
				rend::InstanceProperties insProps;
				geom::MeshIntersection insHit;
				bool hit;
			};
		public:
			[[nodiscard]] CollectedData castRayAtMouse(std::unique_ptr<rend::EngineCamera>& camPtr) const;
		};
	} // cast
} // engn