#pragma once

#include "render/Materials/Material.h"
#include "source/math/geometry/hitable.h"


namespace engn {

struct RenderObject {
	RenderObject(math::hitable* shapePtr, const Material& mat) :
		material{ mat }
	{
		shape = shapePtr;
	}

	~RenderObject() {
		delete shape;
	}

	math::hitable* shape;
	Material material;
};

} // engn