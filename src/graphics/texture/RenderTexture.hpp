#ifndef GRAPHICS_RENDERTEXTURE_HPP
#define GRAPHICS_RENDERTEXTURE_HPP

#include "Texture2D.hpp"

namespace hexeract
{
	namespace graphics
	{
		class FrameBuffer;
		class RenderBuffer;

		class RenderTexture : public Texture2D
		{
		public:
			enum depth_format
			{
				depth_none = 0,
				depth_16 = GL_DEPTH_COMPONENT16,
				depth_24 = GL_DEPTH_COMPONENT24,
				depth_32 = GL_DEPTH_COMPONENT32,
				depth_32f = GL_DEPTH_COMPONENT32F,
				depth_24stencil8 = GL_DEPTH24_STENCIL8,
			};

			RenderTexture();

			FrameBuffer* fbo() { return m_fbo.get(); }
			const FrameBuffer* fbo() const { return m_fbo.get(); }

			depth_format depthFormat() const { return m_depthFormat; }

			void setData(unsigned int w, unsigned int h, GLint imgFormat, GLenum pxFormat, GLenum pxType, depth_format depthFmt = depth_none);

			template<typename T>
			void setData(unsigned int w, unsigned int h, depth_format depthFmt = depth_none)
			{
				setData(w, h, image_format<T>(), pixel_format<T>(), pixel_type<T>(), depthFmt);
			}

			template<typename T>
			void setImgFormat()
			{
				setImgFormat(image_format<T>());
			}

		private:
			depth_format m_depthFormat;

			std::unique_ptr<FrameBuffer> m_fbo;
			std::unique_ptr<RenderBuffer> m_depthBuffer;
		};
	}
}

#endif // GRAPHICS_RENDERTEXTURE_HPP