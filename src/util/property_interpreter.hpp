#ifndef PROPERTY_INTERPRETER_HPP
#define PROPERTY_INTERPRETER_HPP

#include <functional>
#include <exception>

#include "keyword_helper.hpp"

template<typename Rt, typename... Args>
class property_interpreter : public keyword_helper<std::function<Rt(Args...)>>
{
public:
	using result_type = Rt;

	explicit property_interpreter(std::initializer_list<value_type> keywords) : keyword_helper(keywords) { }

	template<typename IterType>
	property_interpreter(IterType first, IterType last) : keyword_helper(first, last) { }

	result_type interpret_property(const key_type& propName, Args... args)
	{
		mapped_type fun;
		if (get(propName, fun)) {
			if (fun) {
				try {
					return fun(std::forward<Args>(args)...);
				} catch (std::exception& e) {
					std::cout << "ERROR: an exception was thrown while interpreting json property \"" << propName << "\": " << e.what() << std::endl;
				}
			}
		}

		return result_type();
	}
};

#endif // PROPERTY_INTERPRETER_HPP