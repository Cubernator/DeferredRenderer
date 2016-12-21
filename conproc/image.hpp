#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "processor.hpp"

class image_processor : public processor
{
public:
	image_processor(const conproc* parent);

protected:
	virtual void process_impl(const fs::path& file, const nlohmann::json& options) override;
};

#endif // IMAGE_HPP