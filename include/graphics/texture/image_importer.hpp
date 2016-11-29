#ifndef IMAGE_IMPORTER_HPP
#define IMAGE_IMPORTER_HPP

#include <memory>

#include "nlohmann/json.hpp"
#include "GL/glew.h"
#include "path.hpp"
#include "pixel_types.hpp"
#include "util/keyword_helper.hpp"

class image_importer
{
public:
	image_importer(const nlohmann::json& options);
	virtual ~image_importer() { };

	const char* getData() const { return getData_impl(); }

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }

	GLenum getPixelFormat() const { return m_format.pixelFormat; }
	GLenum getPixelType() const { return m_format.pixelType; }
	std::size_t getPixelSize() const { return m_format.pixelSize; }

	bool getStatus() const { return m_status; }

	GLint getDefaultImageFormat() const {
		return m_format.imageFormat;
	}

	GLint getImageFormat() const {
		return (m_imgFormatOverride != 0) ? m_imgFormatOverride : getDefaultImageFormat();
	}

protected:
	virtual const char* getData_impl() const = 0;

	void setFormat(const pixel_traits& format) { m_format = format; }
	void setStatus(bool status) { m_status = status; }
	void setSize(unsigned int w, unsigned int h)
	{
		m_width = w;
		m_height = h;
	}

	void setSuccess() { setStatus(true); }

private:
	unsigned int m_width, m_height;
	pixel_traits m_format;
	GLint m_imgFormatOverride;
	bool m_status;
};

template<typename T>
struct image_importer_factory
{
	static_assert(std::is_base_of<image_importer, T>::value, "T must be derived from image_importer!");

	std::unique_ptr<image_importer> operator() (const path& file, const nlohmann::json& options) const
	{
		return std::make_unique<T>(file, options);
	}
};

#endif // IMAGE_IMPORTER_HPP