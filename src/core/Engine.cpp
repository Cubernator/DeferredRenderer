#include "Engine.hpp"

#include "Scene.hpp"
#include "Entity.hpp"

#include "ObjectRegistry.hpp"
#include "graphics/RenderEngine.hpp"
#include "content/Content.hpp"
#include "input/Input.hpp"
#include "scripting/Environment.hpp"
#include "scripting/class_registry.hpp"

#include "app_info.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>

Engine::Engine() : m_error(0), m_running(true), m_time(0), m_frameTime(0), m_loadingScene(false)
{
	glfwSetErrorCallback([](int err, const char* msg) {
		std::cout << msg << std::endl;
	});

	// initialize GLFW
	if (!glfwInit()) {
		m_error = -1;
		std::cout << "ERROR: Failed to initialize GLFW!" << std::endl;
		return;
	}

	auto title = app_info::get<std::string>("title", "Untitled");
	auto resolution = app_info::get("resolution", glm::ivec2(640, 480));
	bool fullscreen = app_info::get("fullscreen", false);
	bool windowedFullscreen = app_info::get("windowedFullscreen", false);

	GLFWmonitor* monitor = nullptr;

	if (fullscreen) {
		monitor = glfwGetPrimaryMonitor();

		if (windowedFullscreen) {
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			resolution = { mode->width, mode->height };
		}
	}

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	// create window
	m_window = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), monitor, nullptr);

	if (!m_window) {
		m_error = -2;
		std::cout << "ERROR: Failed to create window!" << std::endl;
		return;
	}

	glfwMakeContextCurrent(m_window);

	setVSync(app_info::get("vSync", false));
	
	glfwSetFramebufferSizeCallback(m_window, resizeCallback);

	// initialize GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		m_error = -3;
		std::cout << "ERROR: Failed to initialize GLEW!" << std::endl;
		return;
	}

	std::cout << "Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	if (!GLEW_VERSION_3_3) {
		m_error = -4;
		std::cout << "ERROR: OpenGL version 3.3 or higher is required!" << std::endl;
		return;
	}

	m_content = std::make_unique<Content>();
	m_scriptEnv = std::make_unique<scripting::Environment>();
	m_objReg = std::make_unique<ObjectRegistry>();
	m_renderer = std::make_unique<RenderEngine>(this);
	m_input = std::make_unique<Input>(m_window);

	m_cmptModules.push_back(m_scriptEnv.get());
	m_cmptModules.push_back(m_renderer.get());

	swapBuffers();

	loadFirstScene();
}

int Engine::run()
{
	auto start = clock::now();
	duration_type accumulator{ 0 };

	while (isRunning()) {
		auto now = clock::now();
		auto newTime = now - start;

		m_frameTime = newTime - m_time;
		m_time = newTime;

		update();
		render();
	}

	return m_error;
}

Engine::~Engine()
{
	m_scene.reset();
	m_input.reset();
	m_renderer.reset();
	m_objReg.reset();
	m_scriptEnv.reset();
	m_content.reset();

	glfwTerminate();
}

bool Engine::isRunning() const
{
	return m_running && !(glfwWindowShouldClose(m_window) || m_error);
}

void Engine::stop()
{
	m_running = false;
}

Scene* Engine::scene()
{
	return m_scene.get();
}

const Scene * Engine::scene() const
{
	return m_scene.get();
}

void Engine::screenSize(int& w, int& h) const
{
	glfwGetWindowSize(m_window, &w, &h);
}

int Engine::screenWidth() const
{
	int w, h;
	screenSize(w, h);
	return w;
}

int Engine::screenHeight() const
{
	int w, h;
	screenSize(w, h);
	return h;
}

void Engine::setVSync(bool val)
{
	m_vSync = val;
	glfwSwapInterval(m_vSync ? 1 : 0);
}

void Engine::loadSceneInternal(const std::string& sceneName)
{
	if (m_scene) {
		std::cout << "Unloading scene \"" << m_scene->name() << "\"..." << std::endl;
		m_scene.reset();
	}

	std::cout << "Loading scene \"" << sceneName << "\"..." << std::endl;
	auto scene = m_content->getFromDisk<Scene>(sceneName);
	if (scene) {
		m_scene = m_objReg->addUnique(std::move(scene));
	} else {
		std::cout << "ERROR: could not find scene " << sceneName << std::endl;
	}

	m_loadingScene = false;
}

void Engine::loadScene(const std::string& sceneName)
{
	m_loadingScene = true;
	m_loadingSceneName = sceneName;
}

void Engine::loadFirstScene()
{
	auto firstSceneName = app_info::get<std::string>("firstScene", "scene0");
	loadScene(firstSceneName);
}

void Engine::addComponent(Component* cmpt)
{
	for (auto cm : m_cmptModules) {
		cm->addComponent(cmpt);
	}
}

void Engine::removeComponent(Component* cmpt)
{
	for (auto cm : m_cmptModules) {
		cm->removeComponent(cmpt);
	}
}

void Engine::onResize(int width, int height)
{
	m_renderer->onResize(width, height);
}

void Engine::update()
{
	if (m_loadingScene) {
		loadSceneInternal(m_loadingSceneName);
	}

	m_input->update();
	m_scriptEnv->update();
}

void Engine::render()
{
	if (m_renderer) {
		m_renderer->render();
	}

	swapBuffers();
}

void Engine::swapBuffers()
{
	glfwSwapBuffers(m_window);
}

SCRIPTING_REGISTER_STATIC_CLASS(Engine)

SCRIPTING_AUTO_MODULE_METHOD(Engine, time)
SCRIPTING_AUTO_MODULE_METHOD(Engine, frameTime)
SCRIPTING_AUTO_MODULE_METHOD(Engine, fps)
SCRIPTING_AUTO_MODULE_METHOD(Engine, screenWidth)
SCRIPTING_AUTO_MODULE_METHOD(Engine, screenHeight)
SCRIPTING_AUTO_MODULE_METHOD(Engine, vSync)
SCRIPTING_AUTO_MODULE_METHOD(Engine, setVSync)
SCRIPTING_AUTO_MODULE_METHOD(Engine, stop)
SCRIPTING_AUTO_MODULE_METHOD(Engine, scene)
SCRIPTING_AUTO_MODULE_METHOD(Engine, loadScene)
SCRIPTING_AUTO_MODULE_METHOD(Engine, loadFirstScene)
