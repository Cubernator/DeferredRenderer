#ifndef JSON_INTERPRETER_HPP
#define JSON_INTERPRETER_HPP

#include "property_interpreter.hpp"
#include "json_utils.hpp"
#include "nlohmann/json.hpp"
#include "boost/iterator/transform_iterator.hpp"

namespace hexeract
{
	template<typename T>
	class json_interpreter : public property_interpreter<void, T*, const nlohmann::json&>
	{
	public:
		using member_func = void (T::*)(const nlohmann::json&);

		template<typename U>
		using setter_func = void (T::*)(U);

		template<typename U>
		using member_var = U T::*;

		template<typename U>
		using kwd_helper = keyword_helper<std::decay_t<U>>;

		struct property_accessor
		{
			mapped_type function;

			// cppcheck-suppress noExplicitConstructor
			property_accessor(member_func f) : function(std::mem_fn(f)) { }

			// cppcheck-suppress noExplicitConstructor
			template<typename U> property_accessor(setter_func<U> f)
				: function(std::bind(setter_from_json<U>(), std::placeholders::_1, f, std::placeholders::_2)) { }

			// cppcheck-suppress noExplicitConstructor
			template<typename U> property_accessor(setter_func<U> f, const kwd_helper<U>* keywords)
				: function(std::bind(setter_from_keyword<U>(), std::placeholders::_1, f, keywords, std::placeholders::_2)) { }

			// cppcheck-suppress noExplicitConstructor
			template<typename U> property_accessor(member_var<U> v)
				: function(std::bind(member_from_json<U>(), std::placeholders::_1, v, std::placeholders::_2)) { }

			// cppcheck-suppress noExplicitConstructor
			template<typename U> property_accessor(member_var<U> v, const kwd_helper<U>* keywords)
				: function(std::bind(member_from_keyword<U>(), std::placeholders::_1, v, keywords, std::placeholders::_2)) { }

			void operator() (T* obj, const nlohmann::json& json) const
			{
				function(obj, json);
			}
		};

		using accessor_pair = std::pair<key_type, property_accessor>;

		// cppcheck-suppress noExplicitConstructor
		json_interpreter(std::initializer_list<accessor_pair> functions)
			: property_interpreter(functions.begin(), functions.end()) { }

		void interpret_all(T* obj, const nlohmann::json& json)
		{
			if (json.is_object()) {
				for (auto it = json.begin(); it != json.end(); ++it) {
					interpret_property(it.key(), obj, it.value());
				}
			}
		}

	private:
		template<typename U>
		struct setter_from_json
		{
			using value_type = std::decay_t<U>;

			void operator() (T* obj, setter_func<U> func, const nlohmann::json& json) const
			{
				(obj->*func)(json_get<value_type>(json));
			}
		};

		template<typename U>
		struct setter_from_keyword
		{
			using value_type = std::decay_t<U>;

			void operator() (T* obj, setter_func<U> func, const kwd_helper<U>* keywords, const nlohmann::json& json) const
			{
				value_type tmp;
				if (keywords->get(json, tmp)) {
					(obj->*func)(std::move(tmp));
				}
			}
		};

		template<typename U>
		struct member_from_json
		{
			using value_type = std::decay_t<U>;

			void operator() (T* obj, member_var<U> var, const nlohmann::json& json) const
			{
				obj->*var = json_get<value_type>(json);
			}
		};

		template<typename U>
		struct member_from_keyword
		{
			using value_type = std::decay_t<U>;

			void operator() (T* obj, member_var<U> var, const kwd_helper<U>* keywords, const nlohmann::json& json) const
			{
				value_type tmp;
				if (keywords->get(json, tmp)) {
					obj->*var = std::move(tmp);
				}
			}
		};
	};
}

#endif // JSON_INTERPRETER_HPP