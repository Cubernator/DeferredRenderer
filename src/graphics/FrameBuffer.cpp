#include "FrameBuffer.hpp"
#include "RenderEngine.hpp"
#include "RenderTarget.hpp"
#include "texture/Texture2D.hpp"
#include "logging/log.hpp"

namespace hexeract
{
	namespace graphics
	{
		FrameBuffer::FrameBuffer() : m_glObj(0), m_clearDepth(1.0f), m_clearStencil(0)
		{
			glGenFramebuffers(1, &m_glObj);
		}

		FrameBuffer::~FrameBuffer()
		{
			glDeleteFramebuffers(1, &m_glObj);
		}

		void FrameBuffer::bind() const
		{
			bind(GL_DRAW_FRAMEBUFFER);
		}

		void FrameBuffer::unbind()
		{
			unbind(GL_DRAW_FRAMEBUFFER);
		}

		void FrameBuffer::bind(GLenum t) const
		{
			glBindFramebuffer(t, m_glObj);
		}

		void FrameBuffer::unbind(GLenum t)
		{
			glBindFramebuffer(t, 0);
		}

		void FrameBuffer::clear() const
		{
			for (unsigned int i = 0; i < colorTargetCount(); ++i) {
				if (m_colorTargets[i])
					glClearBufferfv(GL_COLOR, i, &m_clearColors[i][0]);
			}

			if (m_depthStencilTarget) {
				glDepthMask(GL_TRUE);
				glClearBufferfi(GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);
			} else {
				if (m_depthTarget) {
					glDepthMask(GL_TRUE);
					glClearBufferfv(GL_DEPTH, 0, &m_clearDepth);
				}
				if (m_stencilTarget) {
					glClearBufferiv(GL_STENCIL, 0, &m_clearStencil);
				}
			}
		}

		void FrameBuffer::attachTargets()
		{
			for (unsigned int i = 0; i < colorTargetCount(); ++i) {
				const auto& t = m_colorTargets[i];
				if (t) {
					t->attach(GL_COLOR_ATTACHMENT0 + i);
				}
			}

			if (m_depthStencilTarget) {
				m_depthStencilTarget->attach(GL_DEPTH_STENCIL_ATTACHMENT);
			} else {
				if (m_depthTarget) {
					m_depthTarget->attach(GL_DEPTH_ATTACHMENT);
				}
				if (m_stencilTarget) {
					m_stencilTarget->attach(GL_STENCIL_ATTACHMENT);
				}
			}
		}

		void FrameBuffer::detachTargets()
		{
			for (unsigned int i = 0; i < colorTargetCount(); ++i) {
				const auto& t = m_colorTargets[i];
				if (t) {
					t->detach(GL_COLOR_ATTACHMENT0 + i);
				}
			}

			if (m_depthStencilTarget) {
				m_depthStencilTarget->detach(GL_DEPTH_STENCIL_ATTACHMENT);
			}
			if (m_depthTarget) {
				m_depthTarget->detach(GL_DEPTH_ATTACHMENT);
			}
			if (m_stencilTarget) {
				m_stencilTarget->detach(GL_STENCIL_ATTACHMENT);
			}
		}

		void FrameBuffer::internalSetTargets(target d, target s, target ds, target_array&& c)
		{
			logging::module_logger lg("Graphics");

			GLint mca = 0;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mca);
			if (GLint(c.size()) >= mca) {
				LOG_ERROR(lg) << "Maximum framebuffer color attachements exceeded!";
				return;
			}

			bind();

			detachTargets();

			m_depthTarget = std::move(d);
			m_stencilTarget = std::move(s);
			m_depthStencilTarget = std::move(ds);
			m_colorTargets = std::move(c);
			m_clearColors.resize(colorTargetCount(), glm::vec4(0.f));

			attachTargets();

			setDrawAll();

			if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				LOG_ERROR(lg) << "Framebuffer is incomplete!";
			}

			unbind();
		}

		void FrameBuffer::setDrawBuffers(const std::vector<GLenum>& bufs)
		{
			glDrawBuffers(GLsizei(bufs.size()), bufs.data());
		}

		void FrameBuffer::setDrawAll()
		{
			std::vector<GLenum> drawBuffers(colorTargetCount());
			std::iota(drawBuffers.begin(), drawBuffers.end(), GL_COLOR_ATTACHMENT0);
			setDrawBuffers(drawBuffers);
		}

		void FrameBuffer::setReadBuffer(GLenum buf)
		{
			glReadBuffer(buf);
		}
	}
}
