#ifndef STB_IMG_IMPORTER_HPP
#define STB_IMG_IMPORTER_HPP

#include "image_importer.hpp"

class stb_img_importer : public image_importer
{
public:
	stb_img_importer(const path& file, const nlohmann::json& options);
	~stb_img_importer();

protected:
	virtual const char* getData_impl() const override { return reinterpret_cast<const char*>(m_data); }

private:
	unsigned char *m_data;
};

#endif // STB_IMG_IMPORTER_HPP