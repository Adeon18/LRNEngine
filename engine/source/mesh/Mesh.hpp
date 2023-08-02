#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "source/math/Ray.hpp"


namespace engn {
	namespace mdl {
		using namespace DirectX;

		class Mesh {
		public:
			struct Triangle {
				uint32_t indices[3];
			};
			std::string name;
			geom::BoundingBox box = geom::BoundingBox::empty();
			std::vector<Vertex> vertices;
			std::vector<Triangle> triangles;

			// The map of paths like: (texture_type, path);
			std::unordered_map<std::string, std::string> texturePaths;

			XMMATRIX meshToModel;
			XMMATRIX meshToModelInv;
		};
	} // mdl
} // engn