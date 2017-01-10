#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include "util/bounds.hpp"

class Drawable
{
public:
	virtual ~Drawable() { }

	virtual void bind() const = 0;
	virtual void unbind() const = 0;
	virtual void draw() const = 0;

	virtual aabb bounds() const = 0;
};

#endif // RENDERABLE_HPP