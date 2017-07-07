#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>

#include "boost/mpl/bool.hpp"
#include "Object.hpp"
#include "util/import.hpp"
#include "util/json_initializable.hpp"
#include "util/json_interpreter.hpp"

#define COMPONENT_SET_MULTIPLE_ALLOWED(v) using multiple_allowed = boost::mpl::##v##_

#define COMPONENT_ALLOW_MULTIPLE COMPONENT_SET_MULTIPLE_ALLOWED(true)
#define COMPONENT_DISALLOW_MULTIPLE COMPONENT_SET_MULTIPLE_ALLOWED(false)


class Entity;

class Component : public Object, public json_initializable<Component>
{
public:
	explicit Component(Entity* parent);
	virtual ~Component() = 0;

	Entity* entity() { return m_parent; }
	const Entity* entity() const { return m_parent; }

	bool isEnabled() const { return m_enabled; }
	void setEnabled(bool val) { m_enabled = val; }

	bool isActiveAndEnabled() const;

	COMPONENT_ALLOW_MULTIPLE;

protected:
	virtual void apply_json_impl(const nlohmann::json& json);

	friend struct json_initializable<Component>;

private:
	Entity* m_parent;
	bool m_enabled;

	static json_interpreter<Component> s_properties;
};

void destroy_component(Component* cmpt);

#endif // COMPONENT_HPP