#include "object_pool.hpp"

#include "boost/format.hpp"

std::string object_pool::getAnonName(unsigned int n) const
{
	auto fmt = boost::format("anonymous_%03u") % n;
	return fmt.str();
}
