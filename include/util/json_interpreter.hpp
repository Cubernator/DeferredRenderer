#ifndef JSON_INTERPRETER_HPP
#define JSON_INTERPRETER_HPP

#include "property_interpreter.hpp"
#include "nlohmann/json.hpp"
#include "boost/iterator/transform_iterator.hpp"

template<typename T>
class json_interpreter : public property_interpreter<void, T*, const nlohmann::json&>
{
public:
	using member_func = void (T::*)(const nlohmann::json&);
	using member_func_pair = std::pair<key_type, member_func>;

	json_interpreter(std::initializer_list<member_func_pair> functions)
		: property_interpreter(
			boost::make_transform_iterator(functions.begin(), transform_pair()),
			boost::make_transform_iterator(functions.end(), transform_pair())) { }

	void interpret_all(T* obj, const nlohmann::json& json)
	{
		if (json.is_object()) {
			for (auto it = json.begin(); it != json.end(); ++it) {
				interpret_property(it.key(), obj, it.value());
			}
		}
	}

private:
	struct transform_pair
	{
		value_type operator() (const member_func_pair& v) const
		{
			return value_type(v.first, std::mem_fn(v.second));
		}
	};
};


#endif // JSON_INTERPRETER_HPP