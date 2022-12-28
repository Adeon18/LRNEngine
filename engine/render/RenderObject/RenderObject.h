#pragma once

#include "render/Materials/Material.h"
#include "source/math/geometry/hitable.h"
#include "source/mesh/Mesh.h"


namespace engn {

struct RenderMathObject {
	RenderMathObject(math::hitable* shapePtr, const mtrl::Material& mat) :
		material{ mat }
	{
		shape = shapePtr;
	}

	~RenderMathObject() {
		delete shape;
	}

	math::hitable* shape;
	mtrl::Material material;
};

struct RenderMeshObject {
	RenderMeshObject(const mesh::Mesh& msh, const mtrl::Material& mat, glm::vec3 mshPos) :
		material{ mat }
	{
		mesh = msh;
		modelMatrix = glm::translate(modelMatrix, mshPos);
	}

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	mesh::Mesh mesh;
	mtrl::Material material;
};

} // engn