#include "Scene.hpp"
#include "Engine.hpp"
#include "Entity.hpp"

Scene::Scene()
{
}

Scene::~Scene()
{
	for (Entity* e : m_entities) {
		engine->destroyEntity(e);
	}
}

void Scene::addEntity(std::unique_ptr<Entity> entity)
{
	engine->addEntity(std::move(entity));
	m_entities.push_back(entity.get());
}
