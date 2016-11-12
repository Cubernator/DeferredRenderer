#ifndef APP_INFO_HPP
#define APP_INFO_HPP

#include <string>

#include "import.hpp"
#include "util/json_initializable.hpp"
#include "util/json_interpreter.hpp"

struct app_info : public json_initializable<app_info>
{
	std::string title;
	path contentDir;
	unsigned int screenWidth, screenHeight;
	bool fullscreen;
	std::string firstScene;

	static const app_info info;

private:
	static json_interpreter<app_info> s_properties;

	void apply_json_impl(const nlohmann::json& json);

	void extractTitle(const nlohmann::json& json);
	void extractContentDir(const nlohmann::json& json);
	void extractResolution(const nlohmann::json& json);
	void extractFullscreen(const nlohmann::json& json);
	void extractFirstScene(const nlohmann::json& json);

	static app_info load();

	friend struct json_initializable<app_info>;
};

#endif // APP_INFO_HPP