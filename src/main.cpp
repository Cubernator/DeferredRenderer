#include "core/Engine.hpp"

#if defined(_WIN32) && !defined(HIDE_CONSOLE)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
	Engine engine;
	return engine.run();
}
