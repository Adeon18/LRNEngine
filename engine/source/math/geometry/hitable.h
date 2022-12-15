#pragma once

#include <glm/glm.hpp>

#include <windows.h>
#include <windowsx.h>

#include "../ray.h"

struct HitEntry
{
	bool isHit;
	float rayT;
	glm::vec3 hitPoint;
	glm::vec3 hitNormal;
};

//! An abstract class for thr hitable object
class hitable
{
public:
	[[nodiscard]] virtual HitEntry hit(const ray& r) const = 0;

	COLORREF m_color = RGB(0, 0, 0);
};