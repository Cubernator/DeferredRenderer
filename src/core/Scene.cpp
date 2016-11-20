#include "core/Scene.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "util/json_utils.hpp"
#include "util/type_registry.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Scene, "scene");

json_interpreter<Scene> Scene::s_properties({
	{ "backColor", &Scene::extractBackColor },
	{ "entities", &Scene::extractEntities }
});

Scene::~Scene()
{
	for (Entity* e : m_entities) {
		Engine::instance()->destroyEntity(e);
	}
}

void Scene::addEntity(std::unique_ptr<Entity> entity)
{
	m_entities.push_back(entity.get());
	Engine::instance()->addEntity(std::move(entity));
}


void Scene::update()
{
	for (auto& e : m_entities) {
		e->update();
	}
}

void Scene::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void Scene::extractBackColor(const nlohmann::json& json)
{
	setBackColor(json_get<glm::vec4>(json));
}

void Scene::extractEntities(const nlohmann::json& json)
{
	for (auto ej : json) {
		addEntity(json_to_object<Entity>(ej));
	}
}
