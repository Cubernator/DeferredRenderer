#include "image.hpp"
#include "conproc.hpp"

#include <set>
#include <functional>

#include "tiffio.h"

#include "squish.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include "stb_image_write.h"

#include "GL/glew.h"

#include "rbt.hpp"
#include "keyword_helper.hpp"

image_processor::image_processor(const conproc* parent) : processor(parent), m_width(0), m_height(0), m_compressed(false), m_comprFlags(0),
	m_inputSuccess(false), m_convertSuccess(false), m_outputSuccess(false) { }

void image_processor::process_impl(const fs::path& file, const nlohmann::json& options)
{
	debug_output() << "reading image file..." << std::endl;

	std::string ext = file.extension().string();
	if (ext == ".tif") {
		importTIFF(file);
	} else {
		importSTB(file);
	}

	if (!m_inputSuccess) {
		return;
	}

	m_compressed = false;
	m_comprFlags = 0;

	auto fit = options.find("format");
	if (fit != options.end() && fit->is_string()) {
		parseFormat(*fit, m_compressed, m_comprFlags);
	}

	if (m_compressed) {
		debug_output() << "compressing image..." << std::endl;
		convertCompressed(m_comprFlags);
	} else {
		debug_output() << "converting image to PNG..." << std::endl;
		convertUncompressed();
	}

	if (!m_convertSuccess) {
		debug_output() << "...failed!" << std::endl;
		return;
	}

	debug_output() << "writing output to file..." << std::endl;

	fs::path outFileName = m_parent->dest_dir() / file.filename();
	outFileName.replace_extension(".rbt");
	writeFile(outFileName, options);

	if (!m_outputSuccess) {
		debug_output() << "...failed!" << std::endl;
	} else {
		debug_output() << "...success!" << std::endl;
	}
}

void image_processor::importTIFF(const fs::path& file)
{
	TIFF* tif = TIFFOpenW(file.c_str(), "r");
	if (tif) {
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &m_width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &m_height);

		std::size_t size = sizeof(uint32) * m_width * m_height;
		uint32* data = (uint32*)_TIFFmalloc(size);

		if (TIFFReadRGBAImageOriented(tif, m_width, m_height, data, ORIENTATION_TOPLEFT)) {
			m_inputData.resize(size);
			_TIFFmemcpy(m_inputData.data(), data, size);
			m_inputSuccess = true;
		}

		_TIFFfree(data);

		TIFFClose(tif);
	} else {
		debug_output() << "failed reading TIFF file: " << file << std::endl;
	}
}

void image_processor::importSTB(const fs::path& file)
{
	int w, h, comp, req_comp = 4;
	unsigned char* data = stbi_load(file.string().c_str(), &w, &h, &comp, req_comp);

	if (data) {
		m_width = w;
		m_height = h;

		std::size_t size = req_comp * m_width * m_height;
		m_inputData.resize(size);
		std::copy(data, data + size, m_inputData.begin());
		m_inputSuccess = true;
	} else {
		debug_output() << "failed reading image file: " << stbi_failure_reason() << std::endl;
	}

	stbi_image_free(data);
}

namespace
{
	keyword_helper<int> g_comprFlags{
		{ "DXT1", squish::kDxt1 },
		{ "DXT3", squish::kDxt3 },
		{ "DXT5", squish::kDxt5 },
		{ "RGBA", 0 }
	};
}

void image_processor::parseFormat(const std::string& fmtStr, bool& isCompressed, int& comprFlags)
{
	comprFlags = 0;
	g_comprFlags.get(fmtStr, comprFlags);
	isCompressed = comprFlags != 0;
}

void image_processor::convertCompressed(int flags)
{
	int size = squish::GetStorageRequirements(m_width, m_height, flags);
	m_outputData.resize(size);
	squish::CompressImage(m_inputData.data(), m_width, m_height, m_outputData.data(), flags | squish::kColourRangeFit);
	m_convertSuccess = true;
}

void image_processor::convertUncompressed()
{
	int len;
	unsigned char *png = stbi_write_png_to_mem(m_inputData.data(), 0, m_width, m_height, 4, &len);
	if (png) {
		m_outputData.resize(len);
		std::copy(png, png + len, m_outputData.begin());
		STBIW_FREE(png);
		m_convertSuccess = true;
	}
}

namespace
{
	keyword_helper<rbt_filter> g_filters{
		{ "point", filter_point },
		{ "bilinear", filter_bilinear },
		{ "trilinear", filter_trilinear }
	};

	keyword_helper<rbt_wrap> g_wraps{
		{ "repeat", wrap_repeat },
		{ "clampToEdge", wrap_clampToEdge },
		{ "clampToBorder", wrap_clampToBorder },
		{ "mirroredRepeat", wrap_mirroredRepeat },
		{ "mirrorClampToEdge", wrap_mirrorClampToEdge }
	};
}

template<typename T>
T find_json_value(const nlohmann::json& json, const std::string& key, const T& def)
{
	auto it = json.find(key);
	if (it != json.end()) {
		try {
			return it->get<T>();
		} catch (std::domain_error&) { }
	}

	return def;
}

void image_processor::writeFile(const fs::path& file, const nlohmann::json& options)
{
	fs::ofstream output(file, std::ios::binary | std::ios::out | std::ios::trunc);
	if (output) {
		rbt_info header{
			m_compressed,
			0,
			0,
			0,
			m_width,
			m_height
		};

		bool linear = find_json_value(options, "linear", false);

		switch (m_comprFlags) {
		case squish::kDxt1:
			header.imgFormat = linear ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			break;
		case squish::kDxt3:
			header.imgFormat = linear ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			break;
		case squish::kDxt5:
			header.imgFormat = linear ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			break;
		case 0:
			header.imgFormat = linear ? GL_RGBA : GL_SRGB_ALPHA;
			header.pxFormat = GL_RGBA;
			header.pxType = GL_UNSIGNED_BYTE;
			break;
		}

		header.params.mipmaps = find_json_value(options, "mipmaps", true);
		header.params.anisotropic = find_json_value(options, "anisotropic", 1.0f);

		header.params.filter = filter_point;
		g_filters.findKeyword(options, "filter", header.params.filter);

		header.params.wrap = wrap_repeat;
		g_wraps.findKeyword(options, "wrap", header.params.wrap);

		std::size_t headerSize = sizeof(header);
		std::size_t dataSize = m_outputData.size();

		output.write(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
		output.write(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

		output.write(reinterpret_cast<char*>(&header), headerSize);
		output.write(reinterpret_cast<char*>(m_outputData.data()), dataSize);

		m_outputSuccess = true;
	}
}
