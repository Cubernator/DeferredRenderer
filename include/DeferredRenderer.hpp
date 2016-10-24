#ifndef DEFERREDRENDERER_HPP
#define DEFERREDRENDERER_HPP

#include <chrono>

#include "glm.hpp"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#define START_WIDTH 1280
#define START_HEIGHT 720

class DeferredRenderer
{
public:
	typedef double time_type;
	typedef std::chrono::duration<time_type> duration_type;

private:
	static DeferredRenderer * s_instance;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void framebufferCallback(GLFWwindow* window, int width, int height);

	GLFWwindow * m_window;
	int m_error;

	typedef std::chrono::high_resolution_clock clock;
	duration_type m_time;
	const duration_type m_deltaTime;

	void onKey(int key, int scancode, int action, int mods);
	void onMouseButton(int button, int action, int mods);
	void onResized(int width, int height);

	void update();
	void render();

public:
	DeferredRenderer();
	~DeferredRenderer();

	time_type getTime() const { return m_time.count(); }
	time_type getDeltaTime() const { return m_deltaTime.count(); }

	int run();
};

#endif // DEFERREDRENDERER_HPP
