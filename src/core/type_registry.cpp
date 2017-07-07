#include "type_registry.hpp"

#include <type_traits>
#include <new>
#include <iostream>

using type_container = type_registry::type_container;


object_type type_registry::s_empty_type;

struct null_obj_importer : public object_importer
{
	virtual std::unique_ptr<NamedObject> importFromFile(const path& p) const { return nullptr; }
	virtual std::unique_ptr<NamedObject> importFromJson(const nlohmann::json& json) const { return nullptr; }
};

object_type::object_type() : id(typeid(void)), name("unknown"), importer(std::make_shared<null_obj_importer>()) { }
object_type::object_type(std::type_index id, const std::string& name, const std::string& extension, std::shared_ptr<object_importer> importer)
	: id(id), name(name), extension(extension), importer(importer) { }


object_type::operator bool() const
{
	return id != typeid(void);
}


void type_registry::registerObjectType(object_type type)
{
	// TODO: print warning upon duplicate names
	s_types.insert(type);

	std::cout << "type registered: " << type.name << std::endl;
}

const object_type& type_registry::findById(std::type_index type)
{
	auto& idx = s_types.get<by_id>();
	auto it = idx.find(type);
	if (it != idx.end()) {
		return *it;
	}
	return s_empty_type;
}

const object_type& type_registry::findByName(const std::string& name)
{
	auto& idx = s_types.get<by_name>();
	auto it = idx.find(name);
	if (it != idx.end()) {
		return *it;
	}
	return s_empty_type;
}

const object_type& type_registry::findByExtension(const std::string& ext)
{
	auto& idx = s_types.get<by_extension>();
	auto it = idx.find(ext);
	if (it != idx.end()) {
		return *it;
	}
	return s_empty_type;
}

const object_type& type_registry::getType(const NamedObject* obj)
{
	return findById(typeid(*obj));
}


static std::aligned_storage_t<sizeof(type_container), alignof(type_container)> g_types_buf;
type_container& type_registry::s_types = reinterpret_cast<type_container&>(g_types_buf);

static int schwarz_counter;

type_registry_initializer::type_registry_initializer()
{
	if (schwarz_counter++ == 0) new (&type_registry::s_types) type_container();
}

type_registry_initializer::~type_registry_initializer()
{
	if (--schwarz_counter == 0) (&type_registry::s_types)->~type_container();
}
