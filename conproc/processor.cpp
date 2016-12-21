#include "processor.hpp"

void processor::process(const fs::path& file)
{
	nlohmann::json options = nlohmann::json::object();

	fs::path optPath = file;
	optPath += ".opt";

	fs::ifstream optFile(optPath);
	if (optFile) {
		try {
			optFile >> options;
		} catch (std::exception&) {
			options.clear();
		}
	}

	process_impl(file, options);
}
