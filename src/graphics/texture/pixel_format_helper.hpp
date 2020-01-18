#ifndef PIXEL_FORMAT_HELPER_HPP
#define PIXEL_FORMAT_HELPER_HPP

#include "pixel_types.hpp"
#include "keyword_helper.hpp"

#include <string>

namespace hexeract
{
	namespace graphics
	{
		struct pixel_format_helper
		{
			static bool find(const std::string& name, pixel_traits& format);

		private:
			static keyword_helper<pixel_traits> s_formats;
		};
	}
}

#endif // PIXEL_FORMAT_HELPER_HPP