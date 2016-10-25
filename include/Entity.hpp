#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "typedefs.hpp"

class Entity
{
private:
	const uuid m_id;

public:
	Entity();

	const uuid& getId() const { return m_id; }
};

#endif // ENTITY_HPP