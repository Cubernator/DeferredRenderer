#include "RenderBuffer.hpp"

void RenderBufferTarget::attach(GLenum attPoint) const
{
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, attPoint, GL_RENDERBUFFER, m_buffer->glObj());
}

void RenderBufferTarget::detach(GLenum attPoint) const
{
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, attPoint, GL_RENDERBUFFER, 0);
}

void RenderBufferTarget::dimensions(unsigned int& width, unsigned int& height) const
{
	width = m_buffer->width();
	height = m_buffer->height();
}
