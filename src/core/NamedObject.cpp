#include "NamedObject.hpp"
#include "ObjectRegistry.hpp"

#include "util/json_utils.hpp"
#include "scripting/class_registry.hpp"

namespace hexeract
{
	NamedObject::NamedObject() : m_id(instance<ObjectRegistry>()->getGUID()) { }

	NamedObject::~NamedObject() { }

	void NamedObject::setName(const std::string& name)
	{
		instance<ObjectRegistry>()->setObjectName(this, name);
	}

	SCRIPTING_REGISTER_CLASS(NamedObject);

	SCRIPTING_AUTO_METHOD(NamedObject, id);
	SCRIPTING_AUTO_METHOD(NamedObject, name);
	SCRIPTING_AUTO_METHOD(NamedObject, setName);
}
