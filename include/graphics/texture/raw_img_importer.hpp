#ifndef RAW_IMG_IMPORTER_HPP
#define RAW_IMG_IMPORTER_HPP

#include "image_importer.hpp"
#include "pixel_types.hpp"
#include "util/keyword_helper.hpp"

#include <vector>

class raw_img_importer : public image_importer
{
public:
	raw_img_importer(const path& file, const nlohmann::json& options);

protected:
	virtual const char* getData_impl() const override { return m_data.data(); }

	virtual unsigned int getWidth_impl() const override { return m_width; }
	virtual unsigned int getHeight_impl() const override { return m_height; }

	virtual GLint getImageFormat_impl() const override { return m_pixelTraits.imageFormat; }
	virtual GLenum getPixelFormat_impl() const override { return m_pixelTraits.pixelFormat; }
	virtual GLenum getPixelType_impl() const override { return m_pixelTraits.pixelType; }

	virtual bool getStatus_impl() const override { return m_status; }

private:
	std::vector<char> m_data;
	unsigned int m_width, m_height;
	pixel_traits m_pixelTraits;
	bool m_status;

	static keyword_helper<pixel_traits> s_formats;
};

#endif // RAW_IMG_IMPORTER_HPP