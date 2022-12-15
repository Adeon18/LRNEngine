#pragma once

#include <glm/glm.hpp>

#include "../ray.h"

struct HitEntry
{
	bool isHit;
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
};

//! An abstract class for thr hitable object
class hitable
{
public:
	[[nodiscard]] virtual HitEntry hit(const ray& r) const = 0;
};