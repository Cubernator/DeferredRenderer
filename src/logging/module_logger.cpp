#include "module_logger.hpp"

logging::module_logger::module_logger(const std::string& name)
{
	add_attribute("Module", boost::log::attributes::make_constant(name));
}
