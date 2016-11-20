#ifndef IMAGE_IMPORTER_HPP
#define IMAGE_IMPORTER_HPP

#include <memory>

#include "nlohmann/json.hpp"
#include "GL/glew.h"
#include "path.hpp"

class image_importer
{
public:
	virtual ~image_importer() { };

	const char* getData() const { return getData_impl(); }

	unsigned int getWidth() const { return getWidth_impl(); }
	unsigned int getHeight() const { return getHeight_impl(); }

	GLint getImageFormat() const { return getImageFormat_impl(); }
	GLenum getPixelFormat() const { return getPixelFormat_impl(); }
	GLenum getPixelType() const { return getPixelType_impl(); }

	bool getStatus() const { return getStatus_impl(); }

protected:
	virtual const char* getData_impl() const = 0;

	virtual unsigned int getWidth_impl() const = 0;
	virtual unsigned int getHeight_impl() const = 0;

	virtual GLint getImageFormat_impl() const = 0;
	virtual GLenum getPixelFormat_impl() const = 0;
	virtual GLenum getPixelType_impl() const = 0;

	virtual bool getStatus_impl() const { return true; };
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