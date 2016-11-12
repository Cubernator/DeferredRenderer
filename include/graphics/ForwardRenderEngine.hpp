#ifndef FORWARDRENDERENGINE_HPP
#define FORWARDRENDERENGINE_HPP

#include "RenderEngine.hpp"

class ForwardRenderEngine : public RenderEngine
{
public:
	ForwardRenderEngine(Engine* parent) : RenderEngine(parent) { }

protected:
	virtual void doRender() override;
};

#endif // FORWARDRENDERENGINE_HPP