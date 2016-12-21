#include "graphics/texture/tiff_img_importer.hpp"

#include "tiffio.h"

tiff_img_importer::tiff_img_importer(const path& file, const nlohmann::json& options) : uncompressed_img_importer(options), m_data(nullptr)
{
	TIFF* tif = TIFFOpenW(file.c_str(), "r");
	if (tif) {
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &m_width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &m_height);

		m_data = (uint32*)_TIFFmalloc(sizeof(uint32) * m_width * m_height);

		if (TIFFReadRGBAImageOriented(tif, m_width, m_height, m_data, ORIENTATION_TOPLEFT)) {
			m_format = get_pixel_traits<pixel::rgba8>();
			setSuccess();
		}

		TIFFClose(tif);
	}
}

tiff_img_importer::~tiff_img_importer()
{
	if (m_data) {
		_TIFFfree(m_data);
	}
}
