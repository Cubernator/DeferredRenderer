#include "RenderEngine.hpp"

RenderEngine::RenderEngine(Engine * parent) : m_parent(parent) { }
RenderEngine::~RenderEngine() { }

void RenderEngine::render()
{
	doRender();
}
