#include "util/intersection_tests.hpp"

bool intersect_aabb_sphere(const aabb& aabb, const glm::vec3 sphereCenter, float sphereRadius)
{
	float sqrDist = 0.0f;

	// find closest point on aabb to the sphere's center
	for (unsigned int a = 0; a < 3; ++a) {
		if (sphereCenter[a] < aabb.min[a]) {
			float val = aabb.min[a] - sphereCenter[a];
			sqrDist += val * val;
		}

		if (sphereCenter[a] > aabb.max[a]) {
			float val = sphereCenter[a] - aabb.max[a];
			sqrDist += val * val;
		}
	}

	// is this point closer than the radius?
	return sqrDist <= (sphereRadius * sphereRadius);
}

