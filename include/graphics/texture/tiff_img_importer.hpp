#ifndef TIFF_IMG_IMPORTER_HPP
#define TIFF_IMG_IMPORTER_HPP

#include "uncompressed_img_importer.hpp"

class tiff_img_importer : public uncompressed_img_importer
{
public:
	tiff_img_importer(const path& file, const nlohmann::json& options);
	~tiff_img_importer();

protected:
	virtual const char* getData() const override { return reinterpret_cast<const char*>(m_data); }

private:
	unsigned int *m_data;
};

#endif // TIFF_IMG_IMPORTER_HPP