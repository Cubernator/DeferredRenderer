#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <memory>
#include <unordered_map>

#include "glm.hpp"
#include "guid.hpp"

using time_type = double;
using duration_type = std::chrono::duration<time_type>;

class Input;
class Content;
class RenderEngine;
class Scene;
class Entity;

namespace scripting
{
	class Environment;
}

struct GLFWwindow;

class Engine
{
public:
	using entity_collection = std::unordered_map<guid, std::unique_ptr<Entity>>;
	using const_entity_iterator = typename entity_collection::const_iterator;

	Engine();
	~Engine();

	time_type getTime() const { return m_time.count(); }
	time_type getFrameTime() const { return m_frameTime.count(); }

	time_type getFPS() const { return 1.0 / getFrameTime(); }

	void getScreenSize(int& w, int& h) const;
	int getScreenWidth() const;
	int getScreenHeight() const;

	int run();

	void stop();

	Scene *getScene();
	const Scene *getScene() const;

	void loadScene(const std::string& sceneName);
	void loadFirstScene();

	Entity* getEntity(const guid& id) { return getEntityInternal(id); }
	const Entity* getEntity(const guid& id) const { return getEntityInternal(id); }

	void addEntity(std::unique_ptr<Entity> entity);
	void destroyEntity(const guid& id);
	void destroyEntity(Entity *entity);

	const_entity_iterator entities_begin() const;
	const_entity_iterator entities_end() const;

	guid getGUID();

	static Engine* instance() { return s_instance; }

private:
	using clock = std::chrono::high_resolution_clock;

	GLFWwindow * m_window;
	int m_error;
	bool m_running;

	duration_type m_time, m_frameTime;

	std::unique_ptr<Input> m_input;
	std::unique_ptr<Content> m_content;
	std::unique_ptr<RenderEngine> m_renderer;
	std::unique_ptr<scripting::Environment> m_scriptEnv;

	std::unique_ptr<Scene> m_scene;
	entity_collection m_entities;
	guid m_nextID;

	bool m_loadingScene;
	std::string m_loadingSceneName;

	static Engine* s_instance;


	Engine(const Engine& other) = delete;
	Engine(Engine&& other) = delete;
	Engine& operator=(const Engine& other) = delete;
	Engine& operator=(Engine&& other) = delete;

	bool isRunning() const;

	void update();

	void render();
	void swapBuffers();

	void setScene(std::unique_ptr<Scene> scene);
	void loadSceneInternal(const std::string& sceneName);

	Entity* getEntityInternal(const guid& id) const;

	void onResize(int width, int height);

	static void resizeCallback(GLFWwindow* window, int width, int height)
	{
		s_instance->onResize(width, height);
	}
};

#endif // ENGINE_HPP
