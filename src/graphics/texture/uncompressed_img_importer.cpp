#include "graphics/texture/uncompressed_img_importer.hpp"
#include "graphics/texture/pixel_format_helper.hpp"
#include "graphics/texture/Texture2D.hpp"

uncompressed_img_importer::uncompressed_img_importer(const nlohmann::json& options) : m_format(get_pixel_traits<pixel::rgb8>()), m_imgFormatOverride(0)
{
	// The internal image format can be overriden via the "convertTo" option.
	// This is useful whenever the image data needs to be converted by OpenGL,
	// for example if the image file uses a packed pixel format but needs to be stored in sRGB color space (OpenGL does not support packed sRGB textures)
	auto foit = options.find("convertTo");
	if (foit != options.end()) {
		pixel_traits formatOverride;
		if (pixel_format_helper::find(*foit, formatOverride)) {
			m_imgFormatOverride = formatOverride.imageFormat;
		}
	}
}

GLint uncompressed_img_importer::imageFormat() const
{
	return (m_imgFormatOverride) ? m_imgFormatOverride : m_format.imageFormat;
}

void uncompressed_img_importer::uploadData_impl(Texture2D* texture) const
{
	texture->setData(getData(), m_width, m_height, imageFormat(), m_format.pixelFormat, m_format.pixelType);
}
