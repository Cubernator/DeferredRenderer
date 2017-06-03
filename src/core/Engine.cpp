#include "Engine.hpp"

#include "Scene.hpp"
#include "Entity.hpp"

#include "graphics/RenderEngine.hpp"
#include "content/Content.hpp"
#include "input/Input.hpp"
#include "scripting/Environment.hpp"

#include "util/app_info.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>

Engine* Engine::s_instance = nullptr;

Engine::Engine() : m_error(0), m_running(true), m_time(0), m_frameTime(0), m_nextID(0), m_loadingScene(false)
{
	s_instance = this;

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

	bool vsync = app_info::get("vSync", false);
	glfwSwapInterval(vsync ? 1 : 0);
	
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

	m_input = std::make_unique<Input>(m_window);
	m_content = std::make_unique<Content>();
	m_renderer = std::make_unique<RenderEngine>(this);
	m_scriptEnv = std::make_unique<scripting::Environment>();

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
	m_entities.clear();
	m_scriptEnv.reset();
	m_renderer.reset();
	m_content.reset();
	m_input.reset();

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

void Engine::getScreenSize(int& w, int& h) const
{
	glfwGetWindowSize(m_window, &w, &h);
}

int Engine::getScreenWidth() const
{
	int w, h;
	getScreenSize(w, h);
	return w;
}

int Engine::getScreenHeight() const
{
	int w, h;
	getScreenSize(w, h);
	return h;
}

Scene* Engine::getScene()
{
	return m_scene.get();
}

const Scene* Engine::getScene() const
{
	return m_scene.get();
}

void Engine::setScene(std::unique_ptr<Scene> scene)
{
	if (m_scene)
		m_scene->setActive(false);

	m_scene = std::move(scene);

	if (m_scene)
		m_scene->setActive(true);
}

void Engine::loadSceneInternal(const std::string & sceneName)
{
	setScene(nullptr);

	auto scene = Content::instance()->getFromDisk<Scene>(sceneName);
	if (scene) {
		setScene(std::move(scene));
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

Entity* Engine::getEntityInternal(const guid& id) const
{
	auto it = m_entities.find(id);
	if (it != m_entities.end())
		return it->second.get();
	return nullptr;
}

void Engine::onResize(int width, int height)
{
	m_renderer->onResize(width, height);
}

void Engine::addEntity(std::unique_ptr<Entity> entity)
{
	auto id = entity->getId();
	auto p = m_entities.emplace(id, std::move(entity));
	auto e = p.first->second.get();
	if (m_renderer) {
		m_renderer->addEntity(e);
	}
}

void Engine::destroyEntity(const guid& id)
{
	auto it = m_entities.find(id);
	if (it != m_entities.end()) {
		if (m_renderer) {
			m_renderer->removeEntity(it->second.get());
		}
		m_entities.erase(it);
	}
}

void Engine::destroyEntity(Entity* entity)
{
	destroyEntity(entity->getId());
}

Engine::const_entity_iterator Engine::entities_begin() const
{
	return m_entities.cbegin();
}

Engine::const_entity_iterator Engine::entities_end() const
{
	return m_entities.cend();
}

guid Engine::getGUID()
{
	return m_nextID++;
}

void Engine::update()
{
	if (m_loadingScene) {
		loadSceneInternal(m_loadingSceneName);
	}

	m_input->update();

	if (m_scene) {
		m_scene->update();
	}
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
