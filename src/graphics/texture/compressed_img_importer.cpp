#include "graphics/texture/compressed_img_importer.hpp"
#include "graphics/texture/Texture2D.hpp"

#include "boost/filesystem.hpp"

compressed_img_importer::compressed_img_importer(const path& file, const nlohmann::json& options)
{
	boost::filesystem::ifstream fs(file, std::ios::binary);
	if (fs) {
		unsigned int dataSize;

		fs.read(reinterpret_cast<char*>(&m_format), sizeof(m_format));
		fs.read(reinterpret_cast<char*>(&m_width), sizeof(m_width));
		fs.read(reinterpret_cast<char*>(&m_height), sizeof(m_height));
		fs.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

		m_data.resize(dataSize);
		fs.read(m_data.data(), dataSize);

		setSuccess();
	} else {
		std::cout << "Compressed texture import error: Could not open file: " << file << std::endl;
	}
}

void compressed_img_importer::uploadData_impl(Texture2D* texture) const
{
	texture->setCompressedData(m_data.data(), m_data.size(), m_width, m_height, m_format);
}
