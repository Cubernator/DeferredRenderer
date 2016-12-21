#ifndef COMPRESSED_IMG_IMPORTER_HPP
#define COMPRESSED_IMG_IMPORTER_HPP

#include "image_importer.hpp"

#include <vector>

class compressed_img_importer : public image_importer
{
public:
	compressed_img_importer(const path& file, const nlohmann::json& options);

protected:
	std::vector<char> m_data;
	GLint m_format;

	virtual void uploadData_impl(Texture2D* texture) const final;
};

#endif // COMPRESSED_IMG_IMPORTER_HPP