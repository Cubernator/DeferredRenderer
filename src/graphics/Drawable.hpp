#ifndef GRAPHICS_DRAWABLE_HPP
#define GRAPHICS_DRAWABLE_HPP

#include "util/bounds.hpp"

namespace hexeract
{
	namespace graphics
	{
		class Drawable
		{
		public:
			virtual ~Drawable() { }

			virtual void bind() const = 0;
			virtual void unbind() const = 0;
			virtual void draw() const = 0;

			virtual aabb bounds() const = 0;
			virtual std::size_t triangles() const = 0;
		};
	}
}

#endif // GRAPHICS_DRAWABLE_HPP