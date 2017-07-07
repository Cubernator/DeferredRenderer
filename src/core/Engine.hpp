#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <memory>
#include <unordered_map>

#include "util/singleton.hpp"
#include "object_pointers.hpp"

#include "glm.hpp"
#include "guid.hpp"

using time_type = double;
using duration_type = std::chrono::duration<time_type>;

class ComponentModule;
class Input;
class ObjectRegistry;
class Content;
class RenderEngine;
class Scene;
class Entity;
class Component;

namespace scripting
{
	class Environment;
}

struct GLFWwindow;

class Engine : public singleton<Engine>
{
public:
	Engine();
	~Engine();

	time_type time() const { return m_time.count(); }
	time_type frameTime() const { return m_frameTime.count(); }

	time_type fps() const { return 1.0 / frameTime(); }

	void screenSize(int& w, int& h) const;
	int screenWidth() const;
	int screenHeight() const;

	bool vSync() const { return m_vSync; }
	void setVSync(bool val);

	int run();

	void stop();

	Scene *scene();
	const Scene *scene() const;

	void loadScene(const std::string& sceneName);
	void loadFirstScene();

	void addComponent(Component* cmpt);
	void removeComponent(Component* cmpt);

private:
	using clock = std::chrono::high_resolution_clock;

	GLFWwindow * m_window;
	int m_error;
	bool m_running;

	bool m_vSync;

	duration_type m_time, m_frameTime;

	std::unique_ptr<Input> m_input;
	std::unique_ptr<ObjectRegistry> m_objReg;
	std::unique_ptr<Content> m_content;
	std::unique_ptr<RenderEngine> m_renderer;
	std::unique_ptr<scripting::Environment> m_scriptEnv;

	std::vector<ComponentModule*> m_cmptModules;

	unique_obj_ptr<Scene> m_scene;

	bool m_loadingScene;
	std::string m_loadingSceneName;


	Engine(const Engine& other) = delete;
	Engine(Engine&& other) = delete;
	Engine& operator=(const Engine& other) = delete;
	Engine& operator=(Engine&& other) = delete;

	bool isRunning() const;

	void update();

	void render();
	void swapBuffers();

	void loadSceneInternal(const std::string& sceneName);

	void onResize(int width, int height);

	static void resizeCallback(GLFWwindow* window, int width, int height)
	{
		instance()->onResize(width, height);
	}
};

#endif // ENGINE_HPP
