#include "NamedObject.hpp"
#include "ObjectRegistry.hpp"

#include "util/json_utils.hpp"
#include "scripting/class_registry.hpp"

NamedObject::NamedObject() : m_id(ObjectRegistry::instance()->getGUID()) { }

NamedObject::~NamedObject() { }

void NamedObject::setName(const std::string& name)
{
	ObjectRegistry::instance()->setObjectName(this, name);
}

SCRIPTING_REGISTER_CLASS(NamedObject)

SCRIPTING_AUTO_METHOD(NamedObject, id)
SCRIPTING_AUTO_METHOD(NamedObject, name)
SCRIPTING_AUTO_METHOD(NamedObject, setName)
