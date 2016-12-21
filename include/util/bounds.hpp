#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include "glm.hpp"

#include <array>

struct sphere
{
	glm::vec3 center;
	float radius;
};

struct obb
{
	glm::vec3 center, extents;
	glm::mat3 axis;

	obb& operator+= (const glm::vec3& t)
	{
		center += t;
		return *this;
	}

	friend obb operator+ (const obb& b, const glm::vec3& t)
	{
		return{ b.center + t, b.extents, b.axis };
	}
};

struct aabb
{
	glm::vec3 min, max;

	glm::vec3 center() const
	{
		return (min + max) * 0.5f;
	}

	glm::vec3 extents() const
	{
		return (max - min) * 0.5f;
	}

	void scale(const glm::vec3& s)
	{
		min *= s;
		max *= s;
	}

	aabb& operator*= (const glm::vec3& s)
	{
		scale(s);
		return *this;
	}

	friend aabb operator* (const aabb& b, const glm::vec3 s)
	{
		return{ b.min * s, b.max * s };
	}

	friend aabb operator* (const glm::vec3 s, const aabb& b)
	{
		return{ b.min * s, b.max * s };
	}
};

inline aabb aabb_union(const aabb& a, const aabb& b)
{
	return { glm::min(a.min, b.min), glm::max(a.max, b.max) };
}

struct plane
{
	glm::vec3 n;
	float d;

	void normalize()
	{
		float rl = 1.0f / glm::length(n);
		n *= rl;
		d *= rl;
	}

	float& operator[] (int i)
	{
		return reinterpret_cast<float*>(this)[i];
	}
};

struct frustum
{
	std::array<plane, 6> planes;

	void normalize()
	{
		for (plane& p : planes) {
			p.normalize();
		}
	}
};

#endif // BOUNDS_HPP