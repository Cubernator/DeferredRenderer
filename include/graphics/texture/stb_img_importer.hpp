#ifndef STB_IMG_IMPORTER_HPP
#define STB_IMG_IMPORTER_HPP

#include "uncompressed_img_importer.hpp"

class stb_img_importer : public uncompressed_img_importer
{
public:
	stb_img_importer(const path& file, const nlohmann::json& options);
	~stb_img_importer();

protected:
	virtual const char* getData() const override { return reinterpret_cast<const char*>(m_data); }

private:
	unsigned char *m_data;
};

#endif // STB_IMG_IMPORTER_HPP