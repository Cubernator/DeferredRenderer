#include "DeferredRenderer.hpp"

#include <iostream>

DeferredRenderer* DeferredRenderer::s_instance = nullptr;

void DeferredRenderer::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	s_instance->onKey(key, scancode, action, mods);
}

void DeferredRenderer::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	s_instance->onMouseButton(button, action, mods);
}

void DeferredRenderer::framebufferCallback(GLFWwindow * window, int width, int height)
{
	s_instance->onResized(width, height);
}

DeferredRenderer::DeferredRenderer() : m_error(0), m_time(0), m_deltaTime(1.0 / 60.0)
{
	glfwSetErrorCallback([](int err, const char* msg) {
		std::cout << msg << std::endl;
	});

	// initialize GLFW
	if (!glfwInit()) {
		m_error = -1;
		return;
	}

	// create window
	m_window = glfwCreateWindow(START_WIDTH, START_HEIGHT, "Deferred Renderer", NULL, NULL);
	if (!m_window) {
		m_error = -2;
		return;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(0);

	// set callbacks
	glfwSetKeyCallback(m_window, DeferredRenderer::keyCallback);
	glfwSetMouseButtonCallback(m_window, DeferredRenderer::mouseButtonCallback);
	glfwSetFramebufferSizeCallback(m_window, DeferredRenderer::framebufferCallback);

	// initialize GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		m_error = -3;
		return;
	}
}

DeferredRenderer::~DeferredRenderer()
{
	glfwTerminate();
}

int DeferredRenderer::run()
{
	auto currentTime = clock::now();
	duration_type accumulator(0);

	while (!glfwWindowShouldClose(m_window) && !m_error) {
		auto newTime = clock::now();
		duration_type frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= m_deltaTime) {
			m_time += m_deltaTime;
			accumulator -= m_deltaTime;

			update();
		}

		render();
	}

	return m_error;
}

void DeferredRenderer::update()
{
	glfwPollEvents();
}

void DeferredRenderer::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(m_window);
}

void DeferredRenderer::onKey(int key, int scancode, int action, int mods)
{
}

void DeferredRenderer::onMouseButton(int button, int action, int mods)
{
}

void DeferredRenderer::onResized(int width, int height)
{
}
