#ifndef LOGGING_SETUP_HPP
#define LOGGING_SETUP_HPP

#include "boost/log/sources/record_ostream.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks.hpp"
#include "boost/log/attributes.hpp"
#include "boost/log/expressions/keyword.hpp"

namespace hexeract
{
	namespace logging
	{
		using text_sink = boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>;

		enum severity_level
		{
			debug,      // verbose debugging information
			info,       // various normal information
			warning,    // warnings
			error,      // non-fatal errors
			critical    // fatal errors resulting in a crash or similar
		};

		using severity_logger = boost::log::sources::severity_logger<severity_level>;

		BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
			BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::log::attributes::local_clock::value_type)
			BOOST_LOG_ATTRIBUTE_KEYWORD(module_attr, "Module", std::string)
			BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

			void init();

		std::ostream& operator<< (std::ostream& strm, severity_level level);
		boost::log::formatting_ostream& operator<< (boost::log::formatting_ostream& strm, boost::log::to_log_manip<severity_level, tag::severity> const& manip);
	}
}

#endif // LOGGING_SETUP_HPP
