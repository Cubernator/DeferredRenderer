#ifndef IMPORT_HPP
#define IMPORT_HPP

#include <istream>
#include <memory>

#include "nlohmann/json.hpp"
#include "boost/filesystem.hpp"

#include "path.hpp"
#include "util/json_initializable.hpp"


template<typename T>
std::unique_ptr<T> json_to_object(const nlohmann::json& json)
{
	static_assert(is_json_initializable<T>::value, "T must be json initializable!");

	auto obj = std::make_unique<T>();
	obj->apply_json(json);
	return std::move(obj);
}

template<typename T>
std::unique_ptr<T> import_object(std::istream& stream)
{
	if (stream) {
		nlohmann::json json;
		json << stream;
		return json_to_object<T>(json);
	}

	return nullptr;
}

template<typename T>
std::unique_ptr<T> import_object(const path& filename)
{
	boost::filesystem::ifstream f(filename);
	return import_object<T>(f);
}


#endif // IMPORT_HPP