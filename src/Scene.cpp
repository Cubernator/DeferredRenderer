#include "Scene.hpp"
#include "Engine.hpp"

Scene::Scene()
{
}

Scene::~Scene()
{
	for (Entity* e : m_entities) {
		engine->destroyEntity(e);
	}
}
