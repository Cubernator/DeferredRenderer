#ifndef UNCOMPRESSED_IMG_IMPORTER_HPP
#define UNCOMPRESSED_IMG_IMPORTER_HPP

#include "image_importer.hpp"

class uncompressed_img_importer : public image_importer
{
public:
	uncompressed_img_importer(const nlohmann::json& options);

	GLint imageFormat() const;

protected:
	pixel_traits m_format;
	GLint m_imgFormatOverride;

	virtual void uploadData_impl(Texture2D* texture) const final;

	virtual const char* getData() const = 0;
};

#endif // UNCOMPRESSED_IMG_IMPORTER_HPP