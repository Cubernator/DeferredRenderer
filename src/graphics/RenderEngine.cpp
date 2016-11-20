#include "graphics/RenderEngine.hpp"

RenderEngine::RenderEngine(Engine * parent) : m_parent(parent) { }
RenderEngine::~RenderEngine() { }

void RenderEngine::render()
{
	onRender();
}

void RenderEngine::addEntity(Entity* entity)
{
	onAddEntity(entity);
}

void RenderEngine::removeEntity(Entity* entity)
{
	onRemoveEntity(entity);
}
