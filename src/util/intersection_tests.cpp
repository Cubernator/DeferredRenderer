#include "intersection_tests.hpp"

bool intersect_aabb_sphere(const aabb& aabb, const sphere& sphere)
{
	float sqrDist = 0.0f;

	// find closest point on aabb to the sphere's center
	for (unsigned int a = 0; a < 3; ++a) {
		if (sphere.center[a] < aabb.min[a]) {
			float val = aabb.min[a] - sphere.center[a];
			sqrDist += val * val;
		}

		if (sphere.center[a] > aabb.max[a]) {
			float val = sphere.center[a] - aabb.max[a];
			sqrDist += val * val;
		}
	}

	// is this point closer than the radius?
	return sqrDist <= (sphere.radius * sphere.radius);
}

bool intersect_obb_frustum(const obb& obb, const frustum& frustum)
{
	for (unsigned int i = 0; i < 6; ++i) {
		const plane& p = frustum.planes[i];

		// d: signed distance between obb center and plane
		float d = glm::dot(obb.center, p.n) + p.d;

		// a: extent of obb along plane normal
		float a = obb.extents.x * glm::abs(glm::dot(p.n, obb.axis[0]))
			+ obb.extents.y * glm::abs(glm::dot(p.n, obb.axis[1]))
			+ obb.extents.z * glm::abs(glm::dot(p.n, obb.axis[2]));

		// if obb is completely on the "negative" side of any plane then it's not inside the frustum
		if (a + d < 0)
			return false;
	}

	// if all plane tests pass then obb is inside or intersecting the frustum
	return true;
}

bool intersect_sphere_frustum(const sphere& sphere, const frustum& frustum)
{
	for (unsigned int i = 0; i < 6; ++i) {
		const plane& p = frustum.planes[i];

		// d: signed distance between sphere center and plane
		float d = glm::dot(sphere.center, p.n) + p.d;

		// if sphere is completely on the "negative" side of any plane then it's not inside the frustum
		if (sphere.radius + d < 0)
			return false;
	}

	// if all plane tests pass then sphere is inside or intersecting the frustum
	return true;
}

