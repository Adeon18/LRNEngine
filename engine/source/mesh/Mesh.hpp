#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <memory>

#include <DirectXMath.h>

#include "MeshHelpers.hpp"

#include "render/Graphics/Vertex.hpp"

namespace engn {
	namespace mdl {
		using namespace DirectX;

		class Mesh {
		public:
			struct Triangle {
				uint32_t indices[3];
			};
			std::string name;
			BoundingBox box = BoundingBox::empty();
			std::vector<Vertex> vertices;
			std::vector<Triangle> triangles;

			XMMATRIX meshToModel;
			XMMATRIX meshToModelInv;

			//std::vector<XMMATRIX> instances;
			//std::vector<XMMATRIX> instancesInv;
		};
	} // mdl
} // engn