#ifndef INTERSECTION_TESTS_HPP
#define INTERSECTION_TESTS_HPP

#include "bounds.hpp"

bool intersect_aabb_sphere(const aabb& aabb, const glm::vec3 sphereCenter, float sphereRadius);

#endif // INTERSECTION_TESTS_HPP