#pragma once

#include "render/Materials/Material.h"
#include "source/math/geometry/hitable.h"


struct RenderObject {
	RenderObject(hitable* shapePtr, const Material& mat) :
		material{ mat }
	{
		shape = shapePtr;
	}

	~RenderObject() {
		delete shape;
	}

	hitable* shape;
	Material material;
};