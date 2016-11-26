#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include "Texture.hpp"
#include "path.hpp"
#include "glm.hpp"

#include "util/json_initializable.hpp"
#include "util/keyword_helper.hpp"
#include "pixel_types.hpp"

class image_importer;

class Texture2D : public Texture, public json_initializable<Texture2D>
{
public:
	enum filter
	{
		filter_point,
		filter_bilinear,
		filter_trilinear
	};

	enum wrap
	{
		wrap_repeat = GL_REPEAT,
		wrap_clampToEdge = GL_CLAMP_TO_EDGE,
		wrap_clampToBorder = GL_CLAMP_TO_BORDER,
		wrap_mirroredRepeat = GL_MIRRORED_REPEAT,
		wrap_mirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
	};

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

	void setParams(bool mipmaps, filter filtering, wrap wrapping, const glm::vec4& borderColor, float anisotropic);
	void setParams(bool mipmaps, filter filtering, wrap wrapping)
	{
		setParams(mipmaps, filtering, wrapping, glm::vec4(0.f), 0.f);
	}

	unsigned int width() const { return m_width; }
	unsigned int height() const { return m_height; }

private:
	unsigned int m_width, m_height;

	using importer_ptr = std::unique_ptr<image_importer>;
	using importer_factory = std::function<importer_ptr(const path&, const nlohmann::json&)>;

	static keyword_helper<importer_factory> s_importers;
	static keyword_helper<filter> s_filters;
	static keyword_helper<wrap> s_wrap;

	void apply_json_impl(const nlohmann::json& json);

	friend struct json_initializable<Texture2D>;
};

#endif // TEXTURE2D_HPP