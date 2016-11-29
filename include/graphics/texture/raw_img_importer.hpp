#ifndef RAW_IMG_IMPORTER_HPP
#define RAW_IMG_IMPORTER_HPP

#include "image_importer.hpp"

#include <vector>

class raw_img_importer : public image_importer
{
public:
	raw_img_importer(const path& file, const nlohmann::json& options);

protected:
	virtual const char* getData_impl() const override { return m_data.data(); }

private:
	std::vector<char> m_data;
};

#endif // RAW_IMG_IMPORTER_HPP