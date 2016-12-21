#ifndef INTERSECTION_TESTS_HPP
#define INTERSECTION_TESTS_HPP

#include "bounds.hpp"

bool intersect_aabb_sphere(const aabb& aabb, const sphere& sphere);

bool intersect_obb_frustum(const obb& obb, const frustum& frustum);
bool intersect_sphere_frustum(const sphere& sphere, const frustum& frustum);

#endif // INTERSECTION_TESTS_HPP