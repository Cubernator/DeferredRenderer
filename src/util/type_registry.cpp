#include "type_registry.hpp"

#include <type_traits>
#include <new>
#include <iostream>

using type_container = type_registry::type_container;

static std::aligned_storage_t<sizeof(type_container), alignof(type_container)> g_types_buf;
type_container& type_registry::s_types = reinterpret_cast<type_container&>(g_types_buf);


registered_type type_registry::s_empty_type;

registered_type::registered_type() : type(typeid(void)) { }
registered_type::registered_type(std::type_index type, const std::string& name, const std::string& extension)
	: type(type), name(name), extension(extension) { }


registered_type::operator bool() const
{
	return type != typeid(void);
}


void type_registry::registerType(registered_type type)
{
	// TODO: print warning upon duplicate names
	s_types.insert(type);

	std::cout << "type registered: " << type.name << std::endl;
}

const registered_type& type_registry::findByType(std::type_index type)
{
	auto& index_by_type = s_types.get<by_type>();
	auto it = index_by_type.find(type);
	if (it != index_by_type.end()) {
		return *it;
	}
	return s_empty_type;
}

const registered_type& type_registry::findByName(const std::string& name)
{
	auto& index_by_name = s_types.get<by_name>();
	auto it = index_by_name.find(name);
	if (it != index_by_name.end()) {
		return *it;
	}
	return s_empty_type;
}

const registered_type& type_registry::findByExtension(const std::string& ext)
{
	auto& index_by_ext = s_types.get<by_extension>();
	auto it = index_by_ext.find(ext);
	if (it != index_by_ext.end()) {
		return *it;
	}
	return s_empty_type;
}


static int schwarz_counter;

type_registry_initializer::type_registry_initializer()
{
	if (schwarz_counter++ == 0) new (&type_registry::s_types) type_container();
}

type_registry_initializer::~type_registry_initializer()
{
	if (--schwarz_counter == 0) (&type_registry::s_types)->~type_container();
}
