#include "RenderTexture.hpp"
#include "graphics/FrameBuffer.hpp"
#include "graphics/RenderBuffer.hpp"

RenderTexture::RenderTexture() : m_depthFormat(depth_none)
{
	m_fbo = std::make_unique<FrameBuffer>();
}

void RenderTexture::setData(unsigned int w, unsigned int h, GLint imgFormat, GLenum pxFormat, GLenum pxType, depth_format depthFmt)
{
	Texture2D::setData(nullptr, w, h, imgFormat, pxFormat, pxType);

	auto ct = make_tex2D_tgt(this);

	if (depthFmt == depth_none) {
		m_depthBuffer.reset();

		m_fbo->setTarget(std::move(ct));
	} else {
		m_depthBuffer = std::make_unique<RenderBuffer>(w, h, depthFmt);

		auto dt = make_renderbuf_tgt(m_depthBuffer.get());

		if (depthFmt == depth_24stencil8) {
			m_fbo->setTargetDepthStencil(std::move(dt), std::move(ct));
		} else {
			m_fbo->setTargetDepth(std::move(dt), std::move(ct));
		}
	}
}
