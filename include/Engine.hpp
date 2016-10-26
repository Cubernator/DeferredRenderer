#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>
#include <unordered_map>

#include "glm.hpp"
#include "typedefs.hpp"

#define START_WIDTH 1280
#define START_HEIGHT 720

class Scene;
class Entity;
class RenderEngine;

struct GLFWwindow;

class Engine
{
public:
	typedef std::unordered_map<uuid, std::unique_ptr<Entity>> entity_collection;
	typedef typename entity_collection::const_iterator const_entity_iterator;

private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void framebufferCallback(GLFWwindow* window, int width, int height);

	GLFWwindow * m_window;
	int m_error;

	typedef std::chrono::high_resolution_clock clock;
	duration_type m_time;
	const duration_type m_deltaTime;

	std::unique_ptr<Scene> m_scene;
	entity_collection m_entities;

	std::unique_ptr<RenderEngine> m_renderer;

	void onKey(int key, int scancode, int action, int mods);
	void onMouseButton(int button, int action, int mods);
	void onResized(int width, int height);

	void update();
	void render();

	Entity* getEntityInternal(const uuid& id) const;

public:
	Engine();
	~Engine();

	time_type getTime() const { return m_time.count(); }
	time_type getDeltaTime() const { return m_deltaTime.count(); }

	int run();

	Scene *getScene();
	const Scene *getScene() const;

	void setScene(std::unique_ptr<Scene> scene);

	Entity* getEntity(const uuid& id) { return getEntityInternal(id); }
	const Entity* getEntity(const uuid& id) const { return getEntityInternal(id); }

	void addEntity(std::unique_ptr<Entity> entity);
	void destroyEntity(const uuid& id);
	void destroyEntity(Entity *entity);

	const_entity_iterator entities_begin() const;
	const_entity_iterator entities_end() const;
};

// Engine singleton
extern Engine *engine;

#endif // ENGINE_HPP
