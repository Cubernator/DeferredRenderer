#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP

class Engine;

class RenderEngine
{
private:
	Engine *m_parent;
	virtual void doRender() = 0;

public:
	RenderEngine(Engine *parent);
	virtual ~RenderEngine() = 0;

	Engine *getParent() { return m_parent; }
	const Engine *getParent() const { return m_parent; }

	void render();
};

#endif // RENDERENGINE_HPP