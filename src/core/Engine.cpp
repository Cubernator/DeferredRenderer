#include "Engine.hpp"
#include "Content.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "graphics/RenderEngine.hpp"
#include "graphics/texture/Texture2D.hpp"
#include "util/app_info.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "core/Input.hpp"

#include <iostream>

Engine* Engine::s_instance = nullptr;

Engine::Engine() : m_error(0), m_running(true), m_time(0), m_deltaTime(1.0 / 60.0)
{
	s_instance = this;

	glfwSetErrorCallback([](int err, const char* msg) {
		std::cout << msg << std::endl;
	});

	// initialize GLFW
	if (!glfwInit()) {
		m_error = -1;
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
		return;
	}

	std::cout << "Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	m_input = std::make_unique<Input>(m_window);
	m_content = std::make_unique<Content>();
	m_renderer = std::make_unique<RenderEngine>(this);

	createDefaultResources();

	auto firstSceneName = app_info::get<std::string>("firstScene", "scene0");
	loadScene(firstSceneName);
}

int Engine::run()
{
	auto currentTime = clock::now();
	duration_type accumulator(0);

	while (isRunning()) {
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

void Engine::createDefaultResources()
{
	pixel::srgb pw{ 255U, 255U, 255U };
	auto texWhite = std::make_unique<Texture2D>();
	texWhite->setParams(false, filter_point, wrap_repeat);
	texWhite->setData(&pw, 1, 1);
	m_content->addToPool("white", std::move(texWhite));

	pixel::srgb pb{ 0U, 0U, 0U };
	auto texBlack = std::make_unique<Texture2D>();
	texBlack->setParams(false, filter_point, wrap_repeat);
	texBlack->setData(&pb, 1, 1);
	m_content->addToPool("black", std::move(texBlack));
}

Engine::~Engine()
{
	m_scene.reset();
	m_entities.clear();
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

Scene * Engine::getScene()
{
	return m_scene.get();
}

const Scene * Engine::getScene() const
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

void Engine::loadScene(const std::string& sceneName)
{
	auto scene = Content::instance()->getFromDisk<Scene>(sceneName);
	if (scene) {
		setScene(std::move(scene));
	} else {
		std::cout << "ERROR: could not find scene " << sceneName << std::endl;
	}
}

Entity * Engine::getEntityInternal(const uuid & id) const
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
	auto p = m_entities.emplace(entity->getId(), std::move(entity));
	if (m_renderer) {
		m_renderer->addEntity(p.first->second.get());
	}
}

void Engine::destroyEntity(const uuid& id)
{
	auto it = m_entities.find(id);
	if (it != m_entities.end()) {
		if (m_renderer) {
			m_renderer->removeEntity(it->second.get());
		}
		m_entities.erase(it);
	}
}

void Engine::destroyEntity(Entity * entity)
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

void Engine::update()
{
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

	glfwSwapBuffers(m_window);
}
