#include "core/Engine.hpp"
#include "util/app_info.hpp"

#include "boost/program_options.hpp"

#if defined(_WIN32) && defined(HIDE_CONSOLE)
#define USE_WINMAIN
#include <windows.h>
#endif


#ifdef USE_WINMAIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif // USE_WINMAIN
{
	using namespace boost::program_options;

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

	app_info::load(aiPath);

	Engine engine;
	return engine.run();
}
