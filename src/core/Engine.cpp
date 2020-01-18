#include "Engine.hpp"

#include "Scene.hpp"
#include "Entity.hpp"

#include "ObjectRegistry.hpp"
#include "graphics/RenderEngine.hpp"
#include "content/Registry.hpp"
#include "input/Input.hpp"
#include "scripting/Environment.hpp"
#include "scripting/class_registry.hpp"

#include "logging/log.hpp"

#include "app_info.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>

namespace hexeract
{
	Engine::Engine() : m_error(0), m_running(true), m_time(0), m_frameTime(0), m_loadingScene(false), m_lg("Core")
	{
		glfwSetErrorCallback(errorCallback);

		LOG_INFO(m_lg) << "Initializing GLFW...";
		if (!glfwInit()) {
			m_error = -1;
			LOG_CRITICAL(m_lg) << "Failed to initialize GLFW!";
			return;
		}

		auto title = app_info::get<std::string>("title", "Untitled");
		auto resolution = app_info::get("resolution", glm::ivec2(640, 480));
		auto fullscreen = app_info::get("fullscreen", false);
		auto windowedFullscreen = app_info::get("windowedFullscreen", false);
		auto vSync = app_info::get("vSync", false);

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

		LOG_INFO(m_lg) << "Creating window...";
		LOG_INFO(m_lg) << "Title: " << title;
		LOG_INFO(m_lg) << "Resolution: " << resolution.x << "x" << resolution.y;
		LOG_INFO(m_lg) << "Mode: " << (fullscreen ? (windowedFullscreen ? "Windowed Fullscreen" : "Fullscreen") : "Windowed");
		LOG_INFO(m_lg) << "VSync: " << (vSync ? "Enabled" : "Disabled");
		m_window = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), monitor, nullptr);
		if (!m_window) {
			m_error = -2;
			LOG_CRITICAL(m_lg) << "Failed to create window!";
			return;
		}

		glfwMakeContextCurrent(m_window);

		setVSync(vSync);

		glfwSetFramebufferSizeCallback(m_window, resizeCallback);

		LOG_INFO(m_lg) << "Initializing GLEW...";
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			m_error = -3;
			LOG_CRITICAL(m_lg) << "Failed to initialize GLEW!";
			return;
		}

		const unsigned char* glVerStr = glGetString(GL_VERSION);
		LOG_INFO(m_lg) << "Using OpenGL version: " << glVerStr;

		if (!GLEW_VERSION_3_3) {
			m_error = -4;
			LOG_CRITICAL(m_lg) << "OpenGL version 3.3 or higher is required!";
			return;
		}

		LOG_INFO(m_lg) << "Constructing modules...";
		m_content = std::make_unique<content::Registry>();
		m_scriptEnv = std::make_unique<scripting::Environment>();
		m_objReg = std::make_unique<ObjectRegistry>();
		m_renderer = std::make_unique<graphics::RenderEngine>(this);
		m_input = std::make_unique<input::Input>(m_window);

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

		LOG_INFO(m_lg) << "Cleaning up modules...";
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
			LOG_INFO(m_lg) << "Unloading scene \"" << m_scene->name() << "\"...";
			m_scene.reset();
		}

		LOG_INFO(m_lg) << "Loading scene \"" << sceneName << "\"...";
		auto scene = m_content->getFromDisk<Scene>(sceneName);
		if (scene) {
			m_scene = m_objReg->addUnique(std::move(scene));
		} else {
			LOG_ERROR(m_lg) << "Could not find scene " << sceneName;
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

	void Engine::onError(int err, const char* msg)
	{
		LOG_ERROR(m_lg) << msg;
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
}
