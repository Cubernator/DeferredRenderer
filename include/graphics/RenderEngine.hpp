#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP

class Engine;
class Entity;

class RenderEngine
{
public:
	RenderEngine(Engine *parent);
	virtual ~RenderEngine() = 0;

	Engine *getParent() { return m_parent; }
	const Engine *getParent() const { return m_parent; }

	void render();

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

protected:
	virtual void onRender() = 0;

	virtual void onAddEntity(Entity* entity) = 0;
	virtual void onRemoveEntity(Entity* entity) = 0;

private:
	Engine *m_parent;
};

#endif // RENDERENGINE_HPP