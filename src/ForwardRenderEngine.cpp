#include "ForwardRenderEngine.hpp"
#include "Engine.hpp"
#include "Scene.hpp"

#include "GL\glew.h"

void ForwardRenderEngine::doRender()
{
	Engine* parent = getParent();
	Scene* scene = parent->getScene();

	glm::vec4 bc = scene->getBackColor();

	glClearColor(bc.r, bc.g, bc.b, bc.a);
	glClear(GL_COLOR_BUFFER_BIT);
}
