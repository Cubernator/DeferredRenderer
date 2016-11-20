#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include "nlohmann\json.hpp"
#include <string>

#include "glm.hpp"

glm::vec4 parse_color(const std::string s);

template<typename T>
struct json_getter
{
	static T get(const nlohmann::json& j)
	{
		return j.get<T>();
	}
};

template<typename T>
void json_get_array(const nlohmann::json& j, T* ptr, std::size_t count)
{
	if (j.is_array() && j.size() >= count) {
		for (std::size_t i = 0; i < count; ++i)
			ptr[i] = json_getter<T>::get(j[i]);
	}
}

template<typename T, glm::precision p>
struct json_getter<glm::tvec2<T, p>>
{
	static glm::tvec2<T, p> get(const nlohmann::json& j)
	{
		glm::tvec2<T, p> result;
		json_get_array(j, &result[0], 2);
		return result;
	}
};

template<typename T, glm::precision p>
struct json_getter<glm::tvec3<T, p>>
{
	static glm::tvec3<T, p> get(const nlohmann::json& j)
	{
		glm::tvec3<T, p> result;
		json_get_array(j, &result[0], 3);
		return result;
	}
};

template<typename T, glm::precision p>
struct json_getter<glm::tvec4<T, p>>
{
	static glm::tvec4<T, p> get(const nlohmann::json& j)
	{
		glm::tvec4<T, p> result;
		json_get_array(j, &result[0], 4);
		return result;
	}
};

template<>
struct json_getter<glm::vec4>
{
	static glm::vec4 get(const nlohmann::json& j)
	{
		glm::vec4 result;
		if (j.is_array()) {
			json_get_array(j, &result[0], 4);
		} else if (j.is_string()) {
			result = parse_color(j);
		}
		return result;
	}
};

template<typename T, glm::precision p>
struct json_getter<glm::tmat4x4<T, p>>
{
	static glm::tmat4x4<T, p> get(const nlohmann::json& j)
	{
		glm::tmat4x4<T, p> result;
		json_get_array(j, &result[0], 4);
		return result;
	}
};

template<typename T, glm::precision p>
struct json_getter<glm::tmat3x3<T, p>>
{
	static glm::tmat3x3<T, p> get(const nlohmann::json& j)
	{
		glm::tmat3x3<T, p> result;
		json_get_array(j, &result[0], 3);
		return result;
	}
};

template<typename T, glm::precision p>
struct json_getter<glm::tmat4x3<T, p>>
{
	static glm::tmat4x3<T, p> get(const nlohmann::json& j)
	{
		glm::tmat4x3<T, p> result;
		json_get_array(j, &result[0], 4);
		return result;
	}
};

template<>
struct json_getter<glm::quat>
{
	static glm::quat get(const nlohmann::json& j)
	{
		glm::vec3 euler = json_getter<glm::vec3>::get(j);
		return glm::angleAxis(glm::radians(euler.x), glm::vec3(1, 0, 0))
			* glm::angleAxis(glm::radians(euler.y), glm::vec3(0, 1, 0))
			* glm::angleAxis(glm::radians(euler.z), glm::vec3(0, 0, 1));
	}
};

template<typename T>
T json_get(const nlohmann::json& j)
{
	return json_getter<T>::get(j);
}


template<typename T>
bool get_value(const nlohmann::json& j, const std::string& key, T& value)
{
	auto it = j.find(key);
	if (it != j.end()) {
		try {
			value = json_get<T>(*it);
			return true;
		} catch (std::domain_error&) { }
	}

	return false;
}

template<typename T>
T get_value(const nlohmann::json& j, const std::string& key, const T& def)
{
	T result;
	if (get_value(j, key, result)) {
		return result;
	}
	return def;
}

inline std::string get_name(const nlohmann::json& j, const std::string& def = "unnamed")
{
	return get_value<std::string>(j, "name", def);
}

inline std::string get_type(const nlohmann::json& j, const std::string& def = "undefined")
{
	return get_value<std::string>(j, "type", def);
}

#endif // JSON_UTILS_HPP