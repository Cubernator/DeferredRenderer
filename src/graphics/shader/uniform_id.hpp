#ifndef GRAPHICS_UNIFORM_ID_HPP
#define GRAPHICS_UNIFORM_ID_HPP

#include <string>

namespace hexeract
{
	namespace graphics
	{
		using uniform_id = std::size_t;

		inline uniform_id uniform_name_to_id(const std::string& name)
		{
			return std::hash<std::string>{}(name);
		}
	}
}

#endif // GRAPHICS_UNIFORM_ID_HPP
