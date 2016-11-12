#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP

class Engine;

class RenderEngine
{
public:
	RenderEngine(Engine *parent);
	virtual ~RenderEngine() = 0;

	Engine *getParent() { return m_parent; }
	const Engine *getParent() const { return m_parent; }

	void render();

protected:
	virtual void doRender() = 0;

private:
	Engine *m_parent;
};

#endif // RENDERENGINE_HPP