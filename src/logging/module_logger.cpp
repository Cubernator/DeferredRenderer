#include "module_logger.hpp"

namespace hexeract
{
	namespace logging
	{
		module_logger::module_logger(const std::string& name)
		{
			add_attribute("Module", boost::log::attributes::make_constant(name));
		}
	}
}
