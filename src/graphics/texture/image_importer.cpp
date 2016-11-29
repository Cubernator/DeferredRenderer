#include "graphics/texture/image_importer.hpp"
#include "graphics/texture/pixel_format_helper.hpp"

image_importer::image_importer(const nlohmann::json& options)
	: m_width(0), m_height(0), m_format(get_pixel_traits<pixel::rgb8>()), m_imgFormatOverride(0), m_status(false)
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
