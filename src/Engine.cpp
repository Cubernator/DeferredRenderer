#include "Engine.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "ForwardRenderEngine.hpp"

#include "SceneImporter.hpp"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <iostream>

Engine *engine = nullptr;

void Engine::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	engine->onKey(key, scancode, action, mods);
}

void Engine::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	engine->onMouseButton(button, action, mods);
}

void Engine::framebufferCallback(GLFWwindow * window, int width, int height)
{
	engine->onResized(width, height);
}

Engine::Engine() : m_error(0), m_time(0), m_deltaTime(1.0 / 60.0)
{
	engine = this;

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
	glfwSetKeyCallback(m_window, Engine::keyCallback);
	glfwSetMouseButtonCallback(m_window, Engine::mouseButtonCallback);
	glfwSetFramebufferSizeCallback(m_window, Engine::framebufferCallback);

	// initialize GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		m_error = -3;
		return;
	}

	m_renderer = std::make_unique<ForwardRenderEngine>(this);

	/*
	auto defaultScene = std::make_unique<Scene>();
	defaultScene->setBackColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	setScene(std::move(defaultScene));
	*/

	// TODO: add a "bootstrapping" json file which contains general information about the application

	SceneImporter imp;
	setScene(imp.fromFile({"content/scenes/scene-test.json"}));
}

Engine::~Engine()
{
	glfwTerminate();
}

int Engine::run()
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
	m_entities.emplace(entity->getId(), std::move(entity));
}

void Engine::destroyEntity(const uuid & id)
{
	m_entities.erase(id);
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
	glfwPollEvents();
}

void Engine::render()
{
	if (m_renderer) {
		m_renderer->render();
	}

	glfwSwapBuffers(m_window);
}

void Engine::onKey(int key, int scancode, int action, int mods)
{
}

void Engine::onMouseButton(int button, int action, int mods)
{
}

void Engine::onResized(int width, int height)
{
}
