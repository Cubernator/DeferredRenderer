#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include "nlohmann\json.hpp"
#include <string>

#include "glm.hpp"

namespace json_utils
{
	glm::vec4 parse_color(const std::string s);
	glm::vec4 convert_to_color(const nlohmann::json& j);

	std::string get_type(const nlohmann::json& j);

	inline bool is_type(const nlohmann::json& j, const std::string& t)
	{
		return get_type(j) == t;
	}
}

#endif // JSON_UTILS_HPP