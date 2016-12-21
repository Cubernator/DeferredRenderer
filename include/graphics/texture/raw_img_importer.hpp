#ifndef RAW_IMG_IMPORTER_HPP
#define RAW_IMG_IMPORTER_HPP

#include "uncompressed_img_importer.hpp"

#include <vector>

class raw_img_importer : public uncompressed_img_importer
{
public:
	raw_img_importer(const path& file, const nlohmann::json& options);

protected:
	virtual const char* getData() const override { return m_data.data(); }

private:
	std::vector<char> m_data;
};

#endif // RAW_IMG_IMPORTER_HPP