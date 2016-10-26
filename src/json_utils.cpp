#include "json_utils.hpp"

#include <sstream>

namespace json_utils
{
	glm::vec4 json_utils::parse_color(const std::string s)
	{
		glm::vec4 color(0.f);

		int maxSize = ((int)s.size() - 1) / 2;

		for (int i = 0; i < maxSize; i++) {
			std::stringstream ss;
			ss << std::hex << s.substr(i * 2 + 1, 2);
			int cv;
			ss >> cv;
			color[i] = cv / 255.0f;
		}

		if (maxSize < 4)
			color.a = 1.0f;

		return color;
	}

	glm::vec4 convert_to_color(const nlohmann::json& j)
	{
		if (j.is_string()) {
			return parse_color(j.get<std::string>());
		}
		return glm::vec4(0.f);
	}

	std::string get_type(const nlohmann::json& j)
	{
		auto it = j.find("type");
		if (it != j.end())
			return it->get<std::string>();
		return "undefined";
	}
}
