#ifndef GRAPHICS_RENDERBUFFER_HPP
#define GRAPHICS_RENDERBUFFER_HPP

#include "GL/glew.h"
#include "texture/pixel_types.hpp"
#include "RenderTarget.hpp"

#include <memory>

namespace hexeract
{
	namespace graphics
	{
		class RenderBuffer
		{
		public:
			RenderBuffer(unsigned int w, unsigned int h, GLenum format) : m_format(format), m_width(w), m_height(h)
			{
				glGenRenderbuffers(1, &m_glObj);
				bind();
				glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
				unbind();
			}

			~RenderBuffer()
			{
				glDeleteRenderbuffers(1, &m_glObj);
			}

			GLuint glObj() const { return m_glObj; }

			GLenum format() const { return m_format; }
			unsigned int width() const { return m_width; }
			unsigned int height() const { return m_height; }

			void bind()
			{
				glBindRenderbuffer(GL_RENDERBUFFER, m_glObj);
			}
			void unbind()
			{
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}

		private:
			GLuint m_glObj;
			GLenum m_format;
			unsigned int m_width, m_height;
		};

		template<typename T>
		std::unique_ptr<RenderBuffer> make_renderbuffer(unsigned int width, unsigned int height)
		{
			return std::make_unique<RenderBuffer>(image_format<T>(), width, height);
		}


		class RenderBufferTarget : public RenderTarget
		{
		public:
			explicit RenderBufferTarget(const RenderBuffer* rbuf) : m_buffer(rbuf) { }

			virtual void attach(GLenum attPoint) const final;
			virtual void detach(GLenum attPoint) const final;
			virtual void dimensions(unsigned int& width, unsigned int& height) const final;

		private:
			const RenderBuffer* m_buffer;
		};

		inline std::unique_ptr<RenderBufferTarget> make_renderbuf_tgt(const RenderBuffer* buffer) {
			return std::make_unique<RenderBufferTarget>(buffer);
		}
	}
}

#endif // GRAPHICS_RENDERBUFFER_HPP