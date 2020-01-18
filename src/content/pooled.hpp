#ifndef CONTENT_POOLED_HPP
#define CONTENT_POOLED_HPP

#include "Registry.hpp"
#include "core/ObjectRegistry.hpp"

namespace hexeract
{
	namespace content
	{
		namespace detail
		{
			template<typename T>
			T* find_int(const object_type& type, const std::string& name)
			{
				return instance<ObjectRegistry>()->findTNFirst<T>(name);
			}

			template<>
			inline NamedObject* find_int<NamedObject>(const object_type& type, const std::string& name)
			{
				return instance<ObjectRegistry>()->findTNFirst(type.id, name);
			}
		}

		NamedObject* get_pooled(const object_type& type, const std::string& name);

		inline NamedObject* get_pooled(std::type_index tid, const std::string& name)
		{
			return get_pooled(type_registry::findById(tid), name);
		}

		inline NamedObject* get_pooled(const std::string& typeName, const std::string& name)
		{
			return get_pooled(type_registry::findByName(typeName), name);
		}

		template<typename T>
		T* get_pooled(const std::string& name)
		{
			T* obj = instance<ObjectRegistry>()->findTNFirst<T>(name);
			if (!obj) {
				auto ptr = instance<Registry>()->getFromDisk<T>(name);
				if (ptr) {
					return instance<ObjectRegistry>()->add(std::move(ptr));
				}
			}
			return obj;
		}

		namespace detail
		{
			template<typename T>
			T* get_pooled_int(const object_type& type, const std::string& name)
			{
				return get_pooled<T>(name);
			}
			template<>
			inline NamedObject* get_pooled_int<NamedObject>(const object_type& type, const std::string& name)
			{
				return get_pooled(type, name);
			}

			template<typename T>
			T* get_pooled_json_int(const object_type& type, const nlohmann::json& json)
			{
				if (json.is_string()) {
					return get_pooled_int<T>(type, json);
				} else if (json.is_object()) {
					std::string name = instance<Registry>()->getObjectName(json);
					T* obj = find_int<T>(type, name);
					if (obj) return obj;

					auto ptr = json_to_object<T>(type, json);
					if (ptr) {
						ptr->setName(name);
						return instance<ObjectRegistry>()->add(std::move(ptr));
					}
				}
				return nullptr;
			}
		}

		template<typename T>
		T* get_pooled_json(const nlohmann::json& json)
		{
			return detail::get_pooled_json_int<T>(type_registry::get<T>(), json);
		}

		NamedObject* get_pooled_json(const object_type& type, const nlohmann::json& json);

		inline NamedObject* get_pooled_json(std::type_index tid, const nlohmann::json& json)
		{
			return get_pooled_json(type_registry::findById(tid), json);
		}
		inline NamedObject* get_pooled_json(const std::string& typeName, const nlohmann::json& json)
		{
			return get_pooled_json(type_registry::findByName(typeName), json);
		}
	}
}
#endif // CONTENT_POOLED_HPP