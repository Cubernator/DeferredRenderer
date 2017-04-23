#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP

#include "GL/glew.h"

class RenderTarget
{
public:
	virtual ~RenderTarget() { };

	virtual void attach(GLenum attPoint) const = 0;
	virtual void detach(GLenum attPoint) const = 0;
	virtual void getDimensions(unsigned int& width, unsigned int& height) const = 0;
};

#endif // RENDERTARGET_HPP