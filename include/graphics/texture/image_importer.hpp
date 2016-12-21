#ifndef IMAGE_IMPORTER_HPP
#define IMAGE_IMPORTER_HPP

#include <memory>

#include "nlohmann/json.hpp"
#include "GL/glew.h"
#include "path.hpp"
#include "pixel_types.hpp"
#include "util/keyword_helper.hpp"

class Texture2D;

class image_importer
{
public:
	image_importer() : m_status(false), m_width(0), m_height(0) { }
	virtual ~image_importer() { }

	void uploadData(Texture2D* texture) const
	{
		uploadData_impl(texture);
	}

	bool getStatus() const { return m_status; }

protected:
	bool m_status;
	unsigned int m_width, m_height;

	virtual void uploadData_impl(Texture2D* texture) const = 0;

	void setSuccess() { m_status = true; }
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