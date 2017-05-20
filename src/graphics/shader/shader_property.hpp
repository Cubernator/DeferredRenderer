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

#include "content/Content.hpp"
#include "util/json_initializable.hpp"
#include "util/property_interpreter.hpp"
#include "util/json_utils.hpp"

#include "uniform_id.hpp"
#include "set_uniform.hpp"
#include "ShaderProgram.hpp"


template<typename C = boost::type_erasure::_self>
struct uniform_settable
{
	static void apply(const ShaderProgram* program, uniform_id id, const C& cont)
	{
		program->setUniform(id, cont);
	}
};

namespace boost {
namespace type_erasure {
	template<typename C, typename Base>
	struct concept_interface<uniform_settable<C>, Base, C> : Base
	{
		void applyTo(const ShaderProgram* program, uniform_id id) const
		{
			call(uniform_settable<C>(), program, id, *this);
		}
	};
}
}


namespace mi = boost::multi_index;


struct shader_property : public json_initializable<shader_property>
{
	using value_type = boost::type_erasure::any<boost::mpl::vector<
		uniform_settable<>,
		boost::type_erasure::typeid_<>,
		boost::type_erasure::copy_constructible<>,
		boost::type_erasure::relaxed
	>>;

	std::string name;
	uniform_id id;
	value_type value;

	template<typename T>
	void set(const T& newValue)
	{
		value = newValue;
	}

	void applyTo(const ShaderProgram* program) const
	{
		value.applyTo(program, id);
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

	template<typename T>
	struct get_pooled_object
	{
		value_type operator() (const nlohmann::json& j) const
		{
			return value_type(Content::instance()->getPooledFromDisk<T>(j));
		}
	};

	template<typename T>
	struct get_pooled_object_json
	{
		value_type operator() (const nlohmann::json& j) const
		{
			return value_type(Content::instance()->getPooledFromJson<T>(j));
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

	template<typename T>
	static type_converter make_object_converter(const std::string& type_name)
	{
		return type_converter{ type_name, typeid(T*), get_pooled_object<T>() };
	}

	template<typename T>
	static type_converter make_object_json_converter(const std::string& type_name)
	{
		return type_converter{ type_name, typeid(T*), get_pooled_object_json<T>() };
	}

	struct by_name { };
	struct by_type { };

	struct converter_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_type>, mi::member<type_converter, std::type_index, &type_converter::type_index>>,
		mi::hashed_unique<mi::tag<by_name>, mi::member<type_converter, std::string, &type_converter::type_name>>
	> { };

	using converter_container = mi::multi_index_container<type_converter, converter_container_indices>;

	static converter_container s_converters;


	// cppcheck-suppress unusedPrivateFunction
	void apply_json_impl(const nlohmann::json& json);

	friend struct json_initializable<shader_property>;
};

namespace detail
{
	struct by_id { };
	struct by_name { };

	struct property_container_indices : public mi::indexed_by<
		mi::hashed_unique<mi::tag<by_id>, mi::member<shader_property, uniform_id, &shader_property::id>>,
		mi::hashed_unique<mi::tag<by_name>, mi::member<shader_property, std::string, &shader_property::name>>
	> { };
}

using shader_property_map = mi::multi_index_container<
	shader_property,
	detail::property_container_indices
>;

#endif // SHADER_PROPERTY_HPP