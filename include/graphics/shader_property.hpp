#ifndef SHADER_PROPERTY_HPP
#define SHADER_PROPERTY_HPP

#include <string>

#include "nlohmann/json.hpp"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/indexed_by.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/type_erasure/typeid_of.hpp"
#include "boost/type_erasure/free.hpp"

#include "util/json_initializable.hpp"
#include "util/property_interpreter.hpp"
#include "util/json_utils.hpp"
#include "set_uniform.hpp"


BOOST_TYPE_ERASURE_FREE((detail)(has_set_uniform), set_uniform, 2)

template<typename T = boost::type_erasure::_self>
using has_set_uniform = detail::has_set_uniform<void(GLint, const T&)>;

namespace mi = boost::multi_index;


struct shader_property : public json_initializable<shader_property>
{
	using value_type = boost::type_erasure::any<boost::mpl::vector<
		has_set_uniform<>,
		boost::type_erasure::typeid_<>,
		boost::type_erasure::copy_constructible<>,
		boost::type_erasure::relaxed
	>>;

	std::string name;
	value_type value;

	template<typename T>
	void set(const T& newValue)
	{
		value = newValue;
	}

	void apply(GLint location) const
	{
		set_uniform(location, value);
	}

	void assign_json(const nlohmann::json& json);
	void assign_json(const std::type_info& type, const nlohmann::json& json);
	void assign_json(const std::string& type_name, const nlohmann::json& json);

	friend bool is_same_type(const shader_property& lhs, const shader_property& rhs)
	{
		return boost::type_erasure::typeid_of(lhs.value) == boost::type_erasure::typeid_of(rhs.value);
	}

private:
	template<typename T>
	struct get_json_value
	{
		value_type operator() (const nlohmann::json& j) const
		{
			return value_type(json_get<T>(j));
		}
	};

	using type_converter_function = std::function<value_type(const nlohmann::json&)>;
	struct type_converter
	{
		std::string type_name;
		std::type_index type_index;
		type_converter_function function;
	};

	template<typename T>
	static type_converter make_converter(const std::string& type_name)
	{
		return type_converter{type_name, typeid(T), get_json_value<T>()};
	}

	struct by_name { };
	struct by_type { };

	struct converter_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_type>, mi::member<type_converter, std::type_index, &type_converter::type_index>>,
		mi::hashed_unique<mi::tag<by_name>, mi::member<type_converter, std::string, &type_converter::type_name>>
	> { };

	using converter_container = mi::multi_index_container<type_converter, converter_container_indices>;

	static converter_container s_converters;


	void apply_json_impl(const nlohmann::json& json);

	friend struct json_initializable<shader_property>;
};

namespace detail
{
	struct by_name { };

	struct property_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_name>, mi::member<shader_property, std::string, &shader_property::name>>
	> { };
}

using shader_property_map = mi::multi_index_container<
	shader_property,
	detail::property_container_indices
>;

#endif // SHADER_PROPERTY_HPP