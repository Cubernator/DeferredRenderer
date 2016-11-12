#ifndef PROPERTY_INTERPRETER_HPP
#define PROPERTY_INTERPRETER_HPP

#include <functional>

#include "keyword_helper.hpp"

template<typename Rt, typename... Args>
class property_interpreter : public keyword_helper<std::function<Rt(Args...)>>
{
public:
	using result_type = Rt;

	property_interpreter(std::initializer_list<value_type> keywords) : keyword_helper(keywords) { }

	template<typename IterType>
	property_interpreter(IterType first, IterType last) : keyword_helper(first, last) { }

	result_type interpret_property(const key_type& propName, Args... args)
	{
		mapped_type fun;
		if (get(propName, fun)) {
			if (fun) {
				return fun(std::forward<Args>(args)...);
			}
		}

		return result_type();
	}
};

#endif // PROPERTY_INTERPRETER_HPP