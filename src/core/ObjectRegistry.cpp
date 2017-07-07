#include "ObjectRegistry.hpp"
#include "scripting/Environment.hpp"
#include "scripting/class_registry.hpp"
#include "graphics/RenderEngine.hpp"
#include "type_registry.hpp"

ObjectRegistry::ObjectRegistry() : m_nextID(0) { }

bool ObjectRegistry::addInt(obj_ptr obj)
{
	if (obj) {
		NamedObject* tmp = obj.get();
		auto p = m_objects.insert(std::move(obj));
		if (p.second) {
			const object_type& type = type_registry::getType(tmp);
			if (type) {
				scripting::Environment::instance()->createObject(type.name, tmp);
			}
		}
		return p.second;
	}
	return false;
}

void ObjectRegistry::destroy(NamedObject* obj)
{
	if (obj) {
		scripting::Environment::instance()->invalidateObject(obj);
		m_objects.erase(obj->id());
	}
}

NamedObject* ObjectRegistry::getById(guid id)
{
	const auto& id_idx = m_objects.get<by_id>();
	auto it = id_idx.find(id);
	if (it != id_idx.end()) {
		return it->get();
	}
	return nullptr;
}

ObjectRegistry::obj_array ObjectRegistry::findNAll(const std::string& name)
{
	const auto& idx = m_objects.get<by_name>();
	auto range = idx.equal_range(name);
	auto tl = [](const obj_ptr& obj) {
		return obj.get();
	};
	return obj_array(boost::make_transform_iterator(range.first, tl), boost::make_transform_iterator(range.second, tl));
}

NamedObject* ObjectRegistry::findNFirst(const std::string& name)
{
	const auto& idx = m_objects.get<by_name>();
	auto it = idx.find(name);
	if (it != idx.end()) {
		return it->get();
	}
	return nullptr;
}

ObjectRegistry::obj_array ObjectRegistry::findTAll(std::type_index tid)
{
	return findTAllInt<NamedObject>(tid);
}

ObjectRegistry::obj_array ObjectRegistry::findTAll(const std::string& typeName)
{
	return findTAll(type_name_to_id(typeName));
}

NamedObject* ObjectRegistry::findTFirst(std::type_index tid)
{
	const auto& idx = m_objects.get<by_type>();
	auto it = idx.find(tid);
	if (it != idx.end()) {
		return it->get();
	}
	return nullptr;
}

NamedObject* ObjectRegistry::findTFirst(const std::string& typeName)
{
	return findTFirst(type_name_to_id(typeName));
}

ObjectRegistry::obj_array ObjectRegistry::findTNAll(std::type_index tid, const std::string& name)
{
	return findTNAllInt<NamedObject>(tid, name);
}

ObjectRegistry::obj_array ObjectRegistry::findTNAll(const std::string& typeName, const std::string& name)
{
	return findTNAll(type_name_to_id(typeName), name);
}

NamedObject* ObjectRegistry::findTNFirst(std::type_index tid, const std::string& name)
{
	const auto& idx = m_objects.get<by_type_and_name>();
	auto it = idx.find(std::make_tuple(tid, name));
	if (it != idx.end()) {
		return it->get();
	}
	return nullptr;
}

NamedObject* ObjectRegistry::findTNFirst(const std::string& typeName, const std::string& name)
{
	return findTNFirst(type_name_to_id(typeName), name);
}

void ObjectRegistry::setObjectName(NamedObject* obj, const std::string& name)
{
	auto it = m_objects.find(obj->id());
	if (it != m_objects.end()) {
		auto& idx = m_objects.get<by_name>();
		auto it2 = m_objects.project<by_name>(it);
		idx.modify_key(it2, [&name](std::string& n) {
			n = name;
		});
	} else {
		obj->m_name = name;
	}
}

SCRIPTING_REGISTER_STATIC_CLASS(ObjectRegistry)

SCRIPTING_AUTO_MODULE_METHOD(ObjectRegistry, getById)

SCRIPTING_AUTO_MODULE_METHOD(ObjectRegistry, findNAll)
SCRIPTING_AUTO_MODULE_METHOD(ObjectRegistry, findNFirst)

SCRIPTING_DEFINE_METHOD(ObjectRegistry, findTAll)
{
	scripting::push_value(L,
		instance<ObjectRegistry>()->findTAll(
			scripting::check_arg<std::string>(L, 1)
		)
	);
	return 1;
}

SCRIPTING_DEFINE_METHOD(ObjectRegistry, findTFirst)
{
	scripting::push_value(L,
		instance<ObjectRegistry>()->findTFirst(
			scripting::check_arg<std::string>(L, 1)
		)
	);
	return 1;
}

SCRIPTING_DEFINE_METHOD(ObjectRegistry, findTNAll)
{
	scripting::push_value(L,
		instance<ObjectRegistry>()->findTNAll(
			scripting::check_arg<std::string>(L, 1),
			scripting::check_arg<std::string>(L, 2)
		)
	);
	return 1;
}

SCRIPTING_DEFINE_METHOD(ObjectRegistry, findTNFirst)
{
	scripting::push_value(L,
		instance<ObjectRegistry>()->findTNFirst(
			scripting::check_arg<std::string>(L, 1),
			scripting::check_arg<std::string>(L, 2)
		)
	);
	return 1;
}
