#include "Scene.hpp"
#include "Entity.hpp"
#include "ObjectRegistry.hpp"
#include "type_registry.hpp"
#include "component_registry.hpp"
#include "util/json_utils.hpp"
#include "scripting/class_registry.hpp"

#include "boost/format.hpp"
#include "lua.hpp"

REGISTER_OBJECT_TYPE_NO_EXT(Scene);

json_interpreter<Scene> Scene::s_properties({
	{ "backColor", &Scene::setBackColor },
	{ "ambientLight", &Scene::setAmbientLight },
	{ "entities", &Scene::extractEntities }
});

Scene::Scene()
{
	// collect all "orphaned" entities
	for (auto e : instance<ObjectRegistry>()->findTAll<Entity>()) {
		addEntity(e);
	}
}

Scene::~Scene()
{
	// destroy all non-persistent entities
	for (auto e : m_entities) {
		if (e->isPersistent()) {
			continue;
		}

		instance<ObjectRegistry>()->destroy(e);
	}
}

void Scene::addEntity(Entity* entity)
{
	m_entities.push_back(entity);
	entity->setParentScene(this);
}

void Scene::removeEntity(Entity* entity)
{
	entity->setParentScene(nullptr);
	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), m_entities.end());
}

void Scene::apply_json_impl(const nlohmann::json& json)
{
	s_properties.interpret_all(this, json);
}

void Scene::extractEntities(const nlohmann::json& json)
{
	if (json.is_array()) {
		auto fmt = boost::format("\r[%1$3i%%] %2%...");
		std::size_t entityCount = json.size(), i = 0;

		auto objreg = instance<ObjectRegistry>();

		std::vector<std::pair<Component*, nlohmann::json>> components;

		// NOTE: entity-creation is split into two passes to allow cross-referencing during initialization

		// first pass: create entities and add components
		for (auto& ej : json) {
			Entity* e = objreg->emplace<Entity>(get_name(ej));
			addEntity(e);
			e->preInit(ej);

			auto it = ej.find("components");
			if (it != ej.end() && it->is_array()) {
				for (auto& cj : *it) {
					auto type = component_registry::findByName(get_type(cj));
					if (type) {
						Component* cmpt = type.factory->add(e);
						components.emplace_back(cmpt, cj);
					}
				}
			}

			int progress = int((float(++i) / float(entityCount)) * 100.0f);
			//std::cout << fmt % progress % "creating entities";
		}

		//std::cout << std::endl;

		i = 0;
		std::size_t cmptCount = components.size();

		// second pass: initialize components
		for (auto p : components) {
			p.first->apply_json(p.second);
			int progress = int((float(++i) / float(cmptCount)) * 100.0f);
			//std::cout << fmt % progress % "initializing components";
		}

		//std::cout << std::endl;
	}
}

SCRIPTING_REGISTER_DERIVED_CLASS(Scene, NamedObject)

SCRIPTING_AUTO_METHOD(Scene, backColor)
SCRIPTING_AUTO_METHOD(Scene, setBackColor)
SCRIPTING_AUTO_METHOD(Scene, ambientLight)
SCRIPTING_AUTO_METHOD(Scene, setAmbientLight)
