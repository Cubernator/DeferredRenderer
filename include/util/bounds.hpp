#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include "glm.hpp"

#include <array>

struct aabb
{
	glm::vec3 min, max;
};

struct obb
{
	glm::vec3 center, extents;
	glm::quat rotation;
};

struct plane
{
	float a, b, c, d;
};

struct frustum
{
	std::array<plane, 6> planes;
};

#endif // BOUNDS_HPP