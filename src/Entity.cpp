#include "Entity.hpp"

#include "boost\uuid\random_generator.hpp"

Entity::Entity() : m_id(boost::uuids::random_generator()()) { }
