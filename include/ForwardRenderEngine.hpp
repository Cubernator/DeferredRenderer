#ifndef FORWARDRENDERENGINE_HPP
#define FORWARDRENDERENGINE_HPP

#include "RenderEngine.hpp"

class ForwardRenderEngine : public RenderEngine
{
private:
	void doRender() override;

public:
	ForwardRenderEngine(Engine* parent) : RenderEngine(parent) { }
};

#endif // FORWARDRENDERENGINE_HPP