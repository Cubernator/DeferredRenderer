#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "glm.hpp"

class Entity;

class Scene
{
private:
	std::vector<Entity*> m_entities;

	glm::vec4 m_backColor;

public:
	Scene();
	~Scene();

	const glm::vec4& getBackColor() const { return m_backColor; }
	void setBackColor(const glm::vec4& c) { m_backColor = c; }
};

#endif // SCENE_HPP