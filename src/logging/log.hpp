#ifndef LOGGING_LOG_HPP
#define LOGGING_LOG_HPP

#include "setup.hpp"

#define LOG(lg, sev) BOOST_LOG_SEV((lg), (sev))

#define LOG_DEBUG(lg)		LOG((lg), logging::debug)
#define LOG_INFO(lg)		LOG((lg), logging::info)
#define LOG_WARNING(lg)		LOG((lg), logging::warning)
#define LOG_ERROR(lg)		LOG((lg), logging::error)
#define LOG_CRITICAL(lg)	LOG((lg), logging::critical)

#endif // LOGGING_LOG_HPP