#ifndef APP_INFO_HPP
#define APP_INFO_HPP

#include <string>

#include "path.hpp"
#include "json_utils.hpp"

struct app_info
{
	template<typename T>
	static bool find(const std::string& propName, T& value)
	{
		auto it = s_properties.find(propName);
		try {
			if (it != s_properties.end()) {
				value = json_get<T>(*it);
				return true;
			}
		} catch (std::domain_error&) { }

		return false;
	}

	template<typename T>
	static T get(const std::string& propName, const T& def)
	{
		T result(def);
		find<T>(propName, result);
		return std::move(result);
	}

	template<typename T>
	static T get(const std::string& propName, T&& def)
	{
		T result(std::move(def));
		find<T>(propName, result);
		return std::move(result);
	}

	template<typename T>
	static T get(const std::string& propName)
	{
		return get<T>(propName, {});
	}

	static void load(const path& p);

private:
	static nlohmann::json s_properties;
};

#endif // APP_INFO_HPP