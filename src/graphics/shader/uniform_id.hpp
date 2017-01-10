#ifndef UNIFORM_ID_HPP
#define UNIFORM_ID_HPP

#include <string>

using uniform_id = std::size_t;

inline uniform_id uniform_name_to_id(const std::string& name)
{
	return std::hash<std::string>{}(name);
}

#endif // UNIFORM_ID_HPP
