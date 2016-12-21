#include "graphics\texture\stb_img_importer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

stb_img_importer::stb_img_importer(const path& file, const nlohmann::json& options) : uncompressed_img_importer(options), m_data(nullptr)
{
	int components = 0;
	auto cit = options.find("components");
	if (cit != options.end() && cit->is_number_integer()) {
		components = *cit;
	}

	int w, h, comp;
	m_data = stbi_load(file.string().c_str(), &w, &h, &comp, components);

	if (components > 0) {
		comp = components;
	}

	if (m_data) {
		m_width = w;
		m_height = h;

		pixel_traits format;
		switch (comp) {
		case 1:
			format = get_pixel_traits<pixel::r8>();
			break;
		case 2:
			format = get_pixel_traits<pixel::rg8>();
			break;
		case 3:
			format = get_pixel_traits<pixel::rgb8>();
			break;
		case 4:
			format = get_pixel_traits<pixel::rgba8>();
			break;

		default:
			return;
			break;
		}

		m_format = format;
		setSuccess();
	} else {
		std::cout << "stb texture import error: " << stbi_failure_reason() << std::endl;
	}
}

stb_img_importer::~stb_img_importer()
{
	stbi_image_free(m_data);
}
