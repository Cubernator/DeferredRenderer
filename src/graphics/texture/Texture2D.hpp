#ifndef GRAPHICS_TEXTURE2D_HPP
#define GRAPHICS_TEXTURE2D_HPP

#include "Texture.hpp"
#include "graphics/RenderTarget.hpp"
#include "pixel_types.hpp"

#include "path.hpp"
#include "glm.hpp"

#include "rbt.hpp"

#include "util/import.hpp"

#include <memory>

namespace hexeract
{
	namespace graphics
	{
		class Texture2D : public Texture
		{
		public:
			using filter = rbt_filter;
			using wrap = rbt_wrap;

			Texture2D();

			void setData(const void* data, unsigned int w, unsigned int h, GLint imgFormat, GLenum pxFormat, GLenum pxType);

			template<typename T>
			void setData(const T* data, unsigned int w, unsigned int h, GLint imgFormat)
			{
				setData(data, w, h, internalFormat, pixel_format<T>(), pixel_type<T>());
			}

			template<typename T>
			void setData(const T* data, unsigned int w, unsigned int h)
			{
				setData(data, w, h, image_format<T>(), pixel_format<T>(), pixel_type<T>());
			}

			void setCompressedData(const void* data, GLsizei dataSize, unsigned int w, unsigned int h, GLint format);

			void setParams(bool mipmaps, filter filtering, wrap wrapping, float anisotropic, const glm::vec4& borderColor);
			void setParams(bool mipmaps, filter filtering, wrap wrapping, float anisotropic)
			{
				setParams(mipmaps, filtering, wrapping, anisotropic, glm::vec4(0.f));
			}
			void setParams(bool mipmaps, filter filtering, wrap wrapping)
			{
				setParams(mipmaps, filtering, wrapping, 1.0f);
			}

			unsigned int width() const { return m_width; }
			unsigned int height() const { return m_height; }

		private:
			unsigned int m_width, m_height;
			bool m_mipmaps;
		};


		class Texture2DTarget : public RenderTarget
		{
		public:
			Texture2DTarget(const Texture2D* texture, int level = 0) : m_texture(texture), m_level(level) { }
			virtual void attach(GLenum attPoint) const final;
			virtual void detach(GLenum attPoint) const final;
			virtual void dimensions(unsigned int& width, unsigned int& height) const final;

		private:
			const Texture2D* m_texture;
			int m_level;
		};
	}

	template<>
	std::unique_ptr<graphics::Texture2D> import_object<graphics::Texture2D>(const path& filename);

	inline std::unique_ptr<graphics::Texture2DTarget> make_tex2D_tgt(const graphics::Texture2D* texture) {
		return std::make_unique<graphics::Texture2DTarget>(texture);
	}
}

#endif // GRAPHICS_TEXTURE2D_HPP