#pragma once

#include "render/Materials/Material.h"
#include "source/math/geometry/hitable.h"
#include "source/mesh/Mesh.h"
#include "source/mesh/TriangleOcTree.h"


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

//! A simple wrapper for rendering and wrapping sped up intersection for a mesh object
struct RenderMeshObject {
	RenderMeshObject(mesh::Mesh* msh, const mtrl::Material& mat, const glm::vec3& mshPos) :
		material{ mat }
	{
		// Mesh data
		mesh = msh;
		collideOcTree.initialize(*mesh);
		// Precalculate matrices
		modelMatrix = glm::translate(modelMatrixInv, mshPos);
		modelMatrixInv = glm::inverse(modelMatrix);
	}

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrixInv = glm::mat4(1.0f);
	mesh::Mesh *mesh;
	// Speed up collison
	mesh::TriangleOctree collideOcTree;
	mtrl::Material material;
};

} // engn