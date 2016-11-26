#ifndef APP_INFO_HPP
#define APP_INFO_HPP

#include <string>

#include "import.hpp"
#include "util/json_utils.hpp"
#include "util/json_initializable.hpp"

struct app_info : public json_initializable<app_info>
{
	static const app_info info;

	template<typename T>
	bool find(const std::string& propName, T& value) const
	{
		auto it = properties.find(propName);
		try {
			if (it != properties.end()) {
				value = json_get<T>(*it);
				return true;
			}
		} catch (std::domain_error&) { }

		return false;
	}

	template<typename T>
	T get(const std::string& propName, const T& def) const
	{
		T result(def);
		find<T>(propName, result);
		return std::move(result);
	}

	template<typename T>
	T get(const std::string& propName, T&& def) const
	{
		T result(std::move(def));
		find<T>(propName, result);
		return std::move(result);
	}

	template<typename T>
	T get(const std::string& propName) const
	{
		return get<T>(propName, {});
	}

private:
	nlohmann::json properties;

	void apply_json_impl(const nlohmann::json& json)
	{
		properties = json;
	}

	static app_info load();

	friend struct json_initializable<app_info>;
};

#endif // APP_INFO_HPP