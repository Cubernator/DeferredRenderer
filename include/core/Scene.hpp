#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>

#include "glm.hpp"
#include "util/import.hpp"
#include "util/json_initializable.hpp"
#include "util/json_interpreter.hpp"

class Entity;

class Scene : public json_initializable<Scene>
{
public:
	Scene() = default;
	~Scene();

	Scene(Scene&& other) = default;
	Scene& operator=(Scene&& other) = default;

	bool isActive() const { return m_active; }
	void setActive(bool val);

	void addEntity(std::unique_ptr<Entity> entity);

	const glm::vec4& getBackColor() const { return m_backColor; }
	void setBackColor(const glm::vec4& c) { m_backColor = c; }

	const glm::vec4& getAmbientLight() const { return m_ambientLight; }
	void setAmbientLight(const glm::vec4& c) { m_ambientLight = c; }

	void update();

private:
	bool m_active;
	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_addEntities;

	glm::vec4 m_backColor;
	glm::vec4 m_ambientLight;

	static json_interpreter<Scene> s_properties;

	void applyAddEntities();

	void apply_json_impl(const nlohmann::json& json);
	void extractEntities(const nlohmann::json& json);

	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;

	friend struct json_initializable<Scene>;
};

#endif // SCENE_HPP