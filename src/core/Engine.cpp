#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "core/Entity.hpp"
#include "core/Input.hpp"
#include "core/Content.hpp"
#include "graphics/RenderEngine.hpp"
#include "util/app_info.hpp"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

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

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	// create window
	m_window = glfwCreateWindow(
		app_info::info.screenWidth,
		app_info::info.screenHeight,
		app_info::info.title.c_str(),
		NULL, NULL);

	if (!m_window) {
		m_error = -2;
		return;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	// initialize GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		m_error = -3;
		return;
	}

	m_input = std::make_unique<Input>(m_window);
	m_content = std::make_unique<Content>(app_info::info.contentDir);
	m_renderer = std::make_unique<RenderEngine>(this);

	setScene(Content::instance()->getFromDisk<Scene>(app_info::info.firstScene));

	if (!m_scene) {
		std::cout << "WARNING: first scene could not be loaded!" << std::endl;
	}
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
	m_scene = std::move(scene);
}

Entity * Engine::getEntityInternal(const uuid & id) const
{
	auto it = m_entities.find(id);
	if (it != m_entities.end())
		return it->second.get();
	return nullptr;
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
