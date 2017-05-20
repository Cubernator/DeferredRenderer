#ifndef RANDOM_MOVEMENT_HPP
#define RANDOM_MOVEMENT_HPP

#include "core/Component.hpp"
#include "util/bounds.hpp"

class Transform;

class RandomMovement : public Component
{
public:
	explicit RandomMovement(Entity* parent);

	void setArea(const aabb& a) { m_area = a; }
	void setMinMaxSpeed(float min, float max)
	{ 
		m_minSpeed = min;
		m_maxSpeed = max;
	}

protected:
	virtual void start_impl() override;
	virtual void update_impl() override;

private:
	Transform* m_transform;

	aabb m_area;
	float m_minSpeed, m_maxSpeed;

	glm::vec3 m_velocity;
};

#endif // RANDOM_MOVEMENT_HPP