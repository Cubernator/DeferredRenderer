#include "generic.hpp"
#include "conproc.hpp"

generic_processor::generic_processor(const conproc* parent) : processor(parent) { }

void generic_processor::process_impl(const fs::path& file, const nlohmann::json& options)
{
	fs::path dst = m_parent->dest_dir() / file.filename();
	fs::copy_file(file, dst, fs::copy_option::overwrite_if_exists);
}
