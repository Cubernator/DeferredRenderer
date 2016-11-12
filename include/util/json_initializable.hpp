#ifndef JSON_INITIALIZABLE_HPP
#define JSON_INITIALIZABLE_HPP

#include <string>
#include <exception>
#include "nlohmann/json.hpp"


template<typename T>
struct json_initializable
{
	void apply_json(const nlohmann::json& json)
	{
		static_cast<T*>(this)->apply_json_impl(json);
	}

	void apply_json_property(const std::string& name, const nlohmann::json& json)
	{
		static_cast<T*>(this)->apply_json_property_impl(name, json);
	}

protected:
	// Default implementation (will be hidden if T contains a function with the same name)
	void apply_json_impl(const nlohmann::json& json)
	{
		if (json.is_object()) {
			for (auto it = json.begin(); it != json.end(); ++it) {
				apply_json_property(it.key(), it.value());
			}
		}
	}

	// Should never be called! Correct implementations will either hide this function or implement apply_json_impl directly
	void apply_json_property_impl(const std::string& name, const nlohmann::json& json)
	{
		throw std::logic_error("Default implementation of apply_json_property_impl has been called! Either this function or apply_json_impl must be hidden by the base class!");
	}
};

template<typename T>
using is_json_initializable = std::is_base_of<json_initializable<T>, T>;

#endif // JSON_INITIALIZABLE_HPP