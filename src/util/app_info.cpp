#include "util/app_info.hpp"

#include "boost/filesystem/fstream.hpp"

nlohmann::json app_info::s_properties(nlohmann::json::object());

void app_info::load(const path& p)
{
	std::cout << "Loading app-info file: " << p << "..." << std::endl;
	boost::filesystem::ifstream file(p);
	if (file) {
		try {
			nlohmann::json j;
			j << file;
			s_properties = j;
		} catch (std::invalid_argument& e) {
			std::cout << "ERROR: an error occured while parsing " << p << ": \"" << e.what() << "\"" << std::endl;
		}
	} else {
		std::cout << "ERROR: could not open " << p << std::endl;
	}
}

