#ifndef GRAPHICS_RENDERTARGET_HPP
#define GRAPHICS_RENDERTARGET_HPP

#include "GL/glew.h"

namespace hexeract
{
	namespace graphics
	{
		class RenderTarget
		{
		public:
			virtual ~RenderTarget() { };

			virtual void attach(GLenum attPoint) const = 0;
			virtual void detach(GLenum attPoint) const = 0;
			virtual void dimensions(unsigned int& width, unsigned int& height) const = 0;
		};
	}
}

#endif // GRAPHICS_RENDERTARGET_HPP