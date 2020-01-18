#ifndef CONTENT_REGISTRY_HPP
#define CONTENT_REGISTRY_HPP

#include "core/type_registry.hpp"
#include "util/singleton.hpp"
#include "util/import.hpp"
#include "util/json_utils.hpp"
#include "logging/module_logger.hpp"
#include "logging/log.hpp"

#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace hexeract
{
	namespace content
	{
		class Registry : public singleton<Registry>
		{
		public:
			Registry();

			const path& contentRoot() const { return m_contentRoot; }

			path findGenericFirst(const std::string& name);
			bool findGenericFirst(const std::string& name, path& result);
			std::vector<path> findGenericAll(const std::string& name);

			bool findObject(std::type_index tid, const std::string& name, path& result);
			path findObject(std::type_index tid, const std::string& name)
			{
				path p;
				findObject(tid, name, p);
				return p;
			}

			std::unique_ptr<NamedObject> getFromDisk(const object_type& type, const std::string& name);
			std::unique_ptr<NamedObject> getFromDisk(std::type_index tid, const std::string& name)
			{
				return getFromDisk(type_registry::findById(tid), name);
			}
			std::unique_ptr<NamedObject> getFromDisk(const std::string& typeName, const std::string& name)
			{
				return getFromDisk(type_registry::findByName(typeName), name);
			}

			template<typename T>
			std::unique_ptr<T> getFromDisk(const std::string& name)
			{
				CHECK_NAMED_OBJECT_TYPE(T);
				return getFromDiskInt<T>(type_registry::get<T>(), name);
			}

			std::unique_ptr<NamedObject> getFromJson(const object_type& type, const nlohmann::json& json);
			std::unique_ptr<NamedObject> getFromJson(std::type_index tid, const nlohmann::json& json)
			{
				return getFromJson(type_registry::findById(tid), json);
			}
			std::unique_ptr<NamedObject> getFromJson(const std::string& typeName, const nlohmann::json& json)
			{
				return getFromJson(type_registry::findByName(typeName), json);
			}

			template<typename T>
			std::unique_ptr<T> getFromJson(const nlohmann::json& json)
			{
				CHECK_NAMED_OBJECT_TYPE(T);
				return getFromJsonInt<T>(type_registry::get<T>(), json);
			}

			bool findShaderFile(const path& p, path& result) const;

			std::string getObjectName(const nlohmann::json& json);

		private:
			using simple_registry = std::unordered_multimap<std::string, path>;
			using sub_registry = std::unordered_map<std::string, path>;
			using registry = std::unordered_map<std::type_index, sub_registry>;

			path m_contentRoot;
			registry m_registry;
			simple_registry m_genericRegistry;

			std::vector<path> m_shaderIncludeDirs;

			bool m_logSearch;
			unsigned int m_logIndentLevel;

			unsigned int m_anonCounter;

			logging::module_logger m_lg;


			std::string getAnonName();

			void scanContentFolder(const path& p);
			void addFile(const path& p);

			template<typename T>
			std::unique_ptr<T> getFromDiskInt(const object_type& type, const std::string& name)
			{
				path p;
				if (type && findObject(type.id, name, p)) {
					try {
						auto obj = import_object<T>(type, p);
						if (obj) obj->setName(name);
						return std::move(obj);
					} catch (std::exception& e) {
						LOG_ERROR(m_lg) << "Error while importing " << type.name << " \"" << name << "\": " << e.what();
						return nullptr;
					}
				}

				LOG_ERROR(m_lg) << "Could not find " << type.name << " \"" << name << "\"";
				return nullptr;
			}

			template<typename T>
			std::unique_ptr<T> getFromJsonInt(const object_type& type, const nlohmann::json& json)
			{
				if (json.is_string()) {
					return getFromDiskInt<T>(type, json);
				} else if (json.is_object() && type) {
					auto obj = json_to_object<T>(type, json);
					if (obj) obj->setName(getObjectName(json));
					return std::move(obj);
				}
				return nullptr;
			}
		};
	}
}

#endif // CONTENT_REGISTRY_HPP