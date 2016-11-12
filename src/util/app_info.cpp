#include "util/app_info.hpp"

#include "boost/filesystem/fstream.hpp"

#include "util/json_utils.hpp"

json_interpreter<app_info> app_info::s_properties({
	{ "title", &app_info::extractTitle },
	{ "contentDir", &app_info::extractContentDir },
	{ "resolution", &app_info::extractResolution },
	{ "fullscreen", &app_info::extractFullscreen },
	{ "firstScene", &app_info::extractFirstScene }
});

const app_info app_info::info(app_info::load());


void app_info::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void app_info::extractTitle(const nlohmann::json& json)
{
	title = json.get<std::string>();
}

void app_info::extractContentDir(const nlohmann::json& json)
{
	contentDir = json.get<path>();
}

void app_info::extractResolution(const nlohmann::json& json)
{
	auto res = json_get<glm::ivec2>(json);
	screenWidth = res.x;
	screenHeight = res.y;
}

void app_info::extractFullscreen(const nlohmann::json& json)
{
	fullscreen = json.get<bool>();
}

void app_info::extractFirstScene(const nlohmann::json& json)
{
	firstScene = json.get<std::string>();
}

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

