#ifndef GRAPHICS_TEXTURE_HPP
#define GRAPHICS_TEXTURE_HPP

#include "GL/glew.h"

#include "core/NamedObject.hpp"

namespace hexeract
{
	namespace graphics
	{
		class Texture : public NamedObject
		{
		public:
			explicit Texture(GLenum target) : m_target(target), m_glObj(0)
			{
				glGenTextures(1, &m_glObj);
			}

			virtual ~Texture()
			{
				if (m_glObj)
					glDeleteTextures(1, &m_glObj);
			}

			GLenum glTarget() const { return m_target; }
			GLuint glObj() const { return m_glObj; }

			void bind() const
			{
				glBindTexture(m_target, m_glObj);
			}

			void unbind() const
			{
				glBindTexture(m_target, 0);
			}

		protected:
			GLenum m_target;
			GLuint m_glObj;
		};
	}
}

#endif // GRAPHICS_TEXTURE_HPP