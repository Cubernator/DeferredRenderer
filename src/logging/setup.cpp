#include "setup.hpp"

#include "boost/log/utility/setup.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/support/date_time.hpp"

namespace logging
{
	using namespace boost::log;
	namespace expr = expressions;

	std::ostream& operator<< (std::ostream& strm, severity_level level)
	{
		static const char* strings[] =
		{
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR",
			"CRITICAL"
		};

		if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
			strm << strings[level];
		else
			strm << static_cast<int>(level);

		return strm;
	}

	formatting_ostream& operator<<
		(
			formatting_ostream& strm,
			to_log_manip<severity_level, tag::severity> const& manip
			)
	{
		static const char* strings[] =
		{
			"DEBG",
			"INFO",
			"WARN",
			"ERRO",
			"CRIT"
		};

		severity_level level = manip.get();
		if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
			strm << strings[level];
		else
			strm << static_cast<int>(level);

		return strm;
	}

	void init()
	{
		register_simple_formatter_factory<severity_level, char>("Severity");

		auto modFmt =
			expr::if_(expr::has_attr(module_attr))
			[
				expr::stream << module_attr << ": "
			];

		add_console_log(
			std::clog,
			keywords::format =
			(
				expr::stream
				<< "[" << severity << "] "
				<< modFmt
				<< expr::smessage
				),
			keywords::filter = severity >= info,
			keywords::auto_flush = true
		);

		add_file_log(
			keywords::file_name = "%Y-%m-%d--%H-%M-%S.log",
			keywords::format =
			(
				expr::stream
				<< expr::format_date_time(timestamp, "[%H:%M:%S.%f]")
				<< "[" << severity << "] "
				<< modFmt
				<< expr::smessage
				),
			keywords::auto_flush = true
		);

		add_common_attributes();
	}
}
