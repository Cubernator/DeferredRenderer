#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>

#include "boost/mpl/bool.hpp"
#include "util/import.hpp"
#include "util/json_initializable.hpp"

#define COMPONENT_SET_MULTIPLE_ALLOWED(v) using multiple_allowed = boost::mpl::##v##_

#define COMPONENT_ALLOW_MULTIPLE COMPONENT_SET_MULTIPLE_ALLOWED(true)
#define COMPONENT_DISALLOW_MULTIPLE COMPONENT_SET_MULTIPLE_ALLOWED(false)


class Entity;

class Component : public json_initializable<Component>
{
public:
	Component(Entity* parent);
	virtual ~Component() = 0;

	Entity* getEntity() { return m_parent; }
	const Entity* getEntity() const { return m_parent; }

	COMPONENT_ALLOW_MULTIPLE;

protected:
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) = 0;

	friend struct json_initializable<Component>;

private:
	Entity* m_parent;
};

#endif // COMPONENT_HPP