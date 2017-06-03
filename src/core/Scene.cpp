#include "Scene.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "util/json_utils.hpp"
#include "util/type_registry.hpp"
#include "scripting/Environment.hpp"

#include "boost/format.hpp"
#include "lua.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Scene, "scene");

json_interpreter<Scene> Scene::s_properties({
	{ "backColor", &Scene::setBackColor },
	{ "ambientLight", &Scene::setAmbientLight },
	{ "entities", &Scene::extractEntities }
});

Scene::Scene() : m_active(false)
{
	/*auto L = scripting::Environment::instance()->state();

	lua_newtable(L);
	lua_setglobal(L, "scene");*/
}

Scene::~Scene()
{
	applyAddEntities();

	for (auto e : m_entities) {
		if (e->isPersistent())
			continue;

		Engine::instance()->destroyEntity(e);
	}
}

void Scene::addEntity(std::unique_ptr<Entity> entity)
{
	m_addEntities.push_back(entity.get());
	entity->setParentScene(this);
	if (m_active)
		entity->start();

	Engine::instance()->addEntity(std::move(entity));
}

void Scene::setActive(bool val)
{
	if (val != m_active) {
		m_active = val;
		if (m_active) {
			applyAddEntities();

			for (auto& e : m_entities) {
				e->start();
			}
		}
	}
}

void Scene::update()
{
	for (auto e : m_entities) {
		e->update();
	}

	applyAddEntities();
}

void Scene::applyAddEntities()
{
	auto it = m_entities.insert(m_entities.end(), m_addEntities.begin(), m_addEntities.end());
	m_addEntities.clear();
}

void Scene::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void Scene::extractEntities(const nlohmann::json& json)
{
	if (json.is_array()) {
		std::size_t entityCount = json.size(), i = 0;

		auto fmt = boost::format("[%1$3i%%]");

		for (auto ej : json) {
			addEntity(json_to_object<Entity>(ej));
			int progress = int((float(++i) / float(entityCount)) * 100.0f);
			std::cout << "\r" << (fmt % progress) << " loading scene... ";
		}

		std::cout << std::endl;
	}
}
