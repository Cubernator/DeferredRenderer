#include "DeferredRenderer.hpp"

#pragma comment(lib, "glew32")
#pragma comment(lib, "glfw3dll")
#pragma comment(lib, "opengl32")

int main()
{
	DeferredRenderer app;

	return app.run();
}