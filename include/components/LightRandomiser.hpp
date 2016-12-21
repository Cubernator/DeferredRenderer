#ifndef LIGHT_RANDOMIZER_HPP
#define LIGHT_RANDOMIZER_HPP

#include "core/Component.hpp"
#include "util/bounds.hpp"

class Light;
class Transform;

class LightRandomizer : public Component
{
public:
	LightRandomizer(Entity* parent);

	void setArea(const aabb& a) { m_area = a; }

protected:
	virtual void start_impl() override;
	virtual void update_impl() override;

private:
	Transform* m_transform;
	Light* m_light;

	aabb m_area;
	glm::vec3 m_velocity;
};

#endif // LIGHT_RANDOMIZER_HPP