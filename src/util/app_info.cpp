#include "util/app_info.hpp"

#include "boost/filesystem/fstream.hpp"

#include "util/json_utils.hpp"

const app_info app_info::info(app_info::load());


app_info app_info::load()
{
	app_info ai;

	// TODO: get path from command line arguments
	// TODO: move appinfo.json into same dir as executable
	path p{"content/appinfo.json"};

	boost::filesystem::ifstream file(p);
	if (file) {
		try {
			nlohmann::json j;
			j << file;
			ai.apply_json(j);
		} catch (std::invalid_argument& e) {
			std::cout << "an error occured while parsing " << p << ": \"" << e.what() << "\"" << std::endl;
		}
	} else {
		std::cout << "could not open " << p << std::endl;
	}

	return ai;
}

