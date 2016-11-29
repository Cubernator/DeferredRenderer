#include "graphics/texture/Texture2D.hpp"
#include "graphics/texture/raw_img_importer.hpp"
#include "graphics/texture/stb_img_importer.hpp"
#include "util/type_registry.hpp"
#include "util/json_utils.hpp"
#include "core/Content.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Texture2D, "texture2D");

keyword_helper<Texture2D::importer_factory> Texture2D::s_importers({
	{ "raw", image_importer_factory<raw_img_importer>() },
	{ "stb", image_importer_factory<stb_img_importer>() }
});

keyword_helper<Texture2D::filter> Texture2D::s_filters({
	{ "point", filter_point },
	{ "bilinear", filter_bilinear },
	{ "trilinear", filter_trilinear }
});

keyword_helper<Texture2D::wrap> Texture2D::s_wrap({
	{ "repeat", wrap_repeat },
	{ "clampToEdge", wrap_clampToEdge },
	{ "clampToBorder", wrap_clampToBorder },
	{ "mirroredRepeat", wrap_mirroredRepeat },
	{ "mirrorClampToEdge", wrap_mirrorClampToEdge },
});

Texture2D::Texture2D() : Texture(GL_TEXTURE_2D), m_width(0), m_height(0) { }

void Texture2D::setData(const void* data, unsigned int w, unsigned int h, GLint imgFormat, GLenum pxFormat, GLenum pxType)
{
	bind();
	glTexImage2D(m_target, 0, imgFormat, w, h, 0, pxFormat, pxType, data);
	unbind();
}

void Texture2D::setParams(bool mipmaps, filter filtering, wrap wrapping, const glm::vec4& borderColor, float anisotropic)
{
	GLenum minFilter, magFilter;
	switch (filtering) {
	case Texture2D::filter_point:
		minFilter = mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
		magFilter = GL_NEAREST;
		break;

	case Texture2D::filter_bilinear:
		minFilter = mipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
		magFilter = GL_LINEAR;
		break;

	case Texture2D::filter_trilinear:
		minFilter = mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
		magFilter = GL_LINEAR;
		break;
	}

	bind();
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wrapping);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wrapping);
	glTexParameterfv(m_target, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
	glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic);
	unbind();
}

void Texture2D::apply_json_impl(const nlohmann::json& json)
{
	bool genMipmaps = get_value<bool>(json, "mipmaps", false);
	float anisotropic = get_value<float>(json, "anisotropic", 0.0f);
	glm::vec4 borderColor = get_value<glm::vec4>(json, "borderColor", glm::vec4(0.0f));

	filter filtering = filter_point;
	s_filters.findKeyword(json, "filter", filtering);

	wrap wrapping = wrap_repeat;
	s_wrap.findKeyword(json, "wrap", wrapping);

	setParams(genMipmaps, filtering, wrapping, borderColor, anisotropic);

	auto fit = json.find("file");
	if (fit != json.end() && fit->is_string()) {
		path file;
		if (Content::instance()->findGenericFirst(*fit, file)) {
			importer_factory make_importer;
			if (s_importers.findKeyword(json, "importer", make_importer)) {
				nlohmann::json importOptions = nlohmann::json::object();
				auto optIt = json.find("importOptions");
				if (optIt != json.end() && optIt->is_object()) {
					importOptions = *optIt;
				}

				importer_ptr importer = make_importer(file, importOptions);
				if (importer->getStatus()) {
					setData(importer->getData(), importer->getWidth(), importer->getHeight(),
						importer->getImageFormat(), importer->getPixelFormat(), importer->getPixelType());

					if (genMipmaps) {
						bind();
						glGenerateMipmap(m_target);
						unbind();
					}
				}
			}
		}
	}
}
