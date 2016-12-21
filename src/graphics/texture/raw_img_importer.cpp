#include "graphics/texture/raw_img_importer.hpp"
#include "graphics/texture/pixel_format_helper.hpp"

#include "util/json_utils.hpp"
#include "boost/filesystem/fstream.hpp"

namespace fs = boost::filesystem;

raw_img_importer::raw_img_importer(const path& file, const nlohmann::json& options) : uncompressed_img_importer(options)
{
	int imgSize;
	if (!get_value<int>(options, "size", imgSize)) {
		std::cout << "Raw texture import error: Missing image size!" << std::endl;
		return;
	}

	m_width = imgSize;
	m_height = imgSize;

	// read file
	fs::ifstream texFile(file, std::ios::binary | std::ios::ate);
	if (texFile) {
		auto size = texFile.tellg();
		m_data.resize(std::size_t(size));

		texFile.seekg(0, std::ios::beg);
		texFile.read(&m_data[0], size);

		// determine format
		auto fit = options.find("format");
		if (fit != options.end() && fit->is_string()) {
			pixel_traits format;
			if (pixel_format_helper::find(*fit, format)) {
				m_format = format;
			}
		}

		// check image dimensions
		std::size_t pixelCount = m_width * m_height;
		auto stride = m_format.pixelSize;
		if ((pixelCount * stride) != size) {
			std::cout << "Raw texture import error: File size does not match format and image size specification!" << std::endl;
			return;
		}

		setSuccess();

	} else {
		std::cout << "Raw texture import error: Could not open file: " << file << std::endl;
	}
}
