#pragma once

#include "render/Materials/Material.h"
#include "source/math/geometry/hitable.h"


namespace engn {

struct RenderObject {
	RenderObject(math::hitable* shapePtr, const mtrl::Material& mat) :
		material{ mat }
	{
		shape = shapePtr;
	}

	~RenderObject() {
		delete shape;
	}

	math::hitable* shape;
	mtrl::Material material;
};

} // engn