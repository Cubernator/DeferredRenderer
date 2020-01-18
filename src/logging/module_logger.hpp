#ifndef LOGGING_MODULE_LOGGER_HPP
#define LOGGING_MODULE_LOGGER_HPP

#include "setup.hpp"

namespace hexeract
{
	namespace logging
	{
		class module_logger : public severity_logger
		{
		public:
			module_logger(const std::string& name);
		};
	}
}

#endif // LOGGING_MODULE_LOGGER_HPP