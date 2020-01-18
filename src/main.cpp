#include "core/Engine.hpp"
#include "core/app_info.hpp"
#include "logging/setup.hpp"
#include "logging/log.hpp"

#include "boost/program_options.hpp"

#if defined(_WIN32)

#if !defined(NDEBUG) && defined(HIDE_CONSOLE) && defined(OVERRIDE_CONSOLE)
#undef HIDE_CONSOLE
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

#if defined(HIDE_CONSOLE)
#define USE_WINMAIN
#include <windows.h>
#endif

#endif // _WIN32


#ifdef USE_WINMAIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif // USE_WINMAIN
{
	using namespace boost::program_options;
	using namespace hexeract;

#ifdef USE_WINMAIN
	std::vector<std::string> args = split_winmain(lpCmdLine);
#else
	std::vector<std::string> args(argv, argv + argc);
#endif // USE_WINMAIN

	path aiPath;

	options_description desc;
	desc.add_options()
		("app-info,a", value<path>(&aiPath)->default_value("app-info.json"), "application info file to use");

	variables_map vm;

	store(command_line_parser(args).options(desc).run(), vm);
	notify(vm);

	logging::init();

	app_info::load(aiPath);

	int status = 0;

	{
		logging::severity_logger lg;

		LOG_INFO(lg) << "Constructing engine...";
		Engine engine;

		status = engine.run();

		LOG_INFO(lg) << "Cleaning up...";
	}

	return status;
}
