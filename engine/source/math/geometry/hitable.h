#pragma once

#include <glm/glm.hpp>

#include <windows.h>
#include <windowsx.h>

#include "../ray.h"


namespace engn {

namespace math {

struct HitEntry
{
	float rayT;
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
};

//! An abstract class for thr hitable object
class hitable
{
public:
	static constexpr float MAX_DIST = 1000.0f;
};

} // math

} // engn
