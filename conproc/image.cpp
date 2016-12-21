#include "image.hpp"

image_processor::image_processor(const conproc* parent) : processor(parent) { }

void image_processor::process_impl(const fs::path& file, const nlohmann::json& options)
{
	// TODO: implement image processor
}

/*std::string image_processor::help_msg_impl() const
{
	return "compresses the image using DXT compression";
}*/
