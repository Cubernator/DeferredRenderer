#include "app_info.hpp"

#include "logging/log.hpp"

#include "boost/filesystem/fstream.hpp"

nlohmann::json app_info::s_properties(nlohmann::json::object());

void app_info::load(const path& p)
{
	logging::severity_logger lg;

	LOG_INFO(lg) << "Loading app-info from " << p << "...";
	boost::filesystem::ifstream file(p);
	if (file) {
		try {
			nlohmann::json j;
			j << file;
			s_properties = j;
		} catch (std::invalid_argument& e) {
			LOG_ERROR(lg) << "An error occured while parsing app-info file: \"" << e.what() << "\"";
		}
	} else {
		LOG_ERROR(lg) << "Could not open app-info file!";
	}
}

