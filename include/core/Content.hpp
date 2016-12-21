#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "util/import.hpp"
#include "util/json_utils.hpp"
#include "util/object_pool.hpp"

class Content
{
public:
	Content();

	const path& contentRoot() const { return m_contentRoot; }

	path findGenericFirst(const std::string& name);
	bool findGenericFirst(const std::string& name, path& result);
	std::vector<path> findGenericAll(const std::string& name);

	path findObject(const std::type_info& type, const std::string& name);

	template<typename T>
	path findObject(const std::string& name)
	{
		return findObject(typeid(T), name);
	}

	template<typename T>
	std::unique_ptr<T> getFromDisk(const std::string& name)
	{
		// TODO: print warning if pointer is empty
		return import_object<T>(findObject<T>(name));
	}

	template<typename T>
	std::unique_ptr<T> getFromJson(const nlohmann::json& json)
	{
		if (json.is_string()) {
			return getFromDisk<T>(json.get<std::string>());
		} else if (json.is_object()) {
			return json_to_object<T>(json);
		}
		return std::unique_ptr<T>();
	}

	template<typename T>
	T* getPooled(const std::string& name)
	{
		return m_pool.get<T>(name);
	}

	template<typename T>
	T* getPooledFromDisk(const std::string& name)
	{
		T* obj = getPooled<T>(name);
		if (!obj) {
			auto ptr = getFromDisk<T>(name);
			if (ptr) {
				return m_pool.add<T>(name, std::move(ptr));
			}
		}
		return obj;
	}

	template<typename T>
	T* getPooledFromJson(const nlohmann::json& json)
	{
		if (json.is_string()) {
			return getPooledFromDisk<T>(json.get<std::string>());
		} else if (json.is_object()) {
			std::string name = getObjectName<T>(json);
			T* obj = getPooled<T>(name);
			if (obj) return obj;

			return m_pool.add<T>(name, json_to_object<T>(json));
		}
		return nullptr;
	}

	template<typename T>
	T* addToPool(const std::string& name, std::unique_ptr<T> obj)
	{
		return m_pool.add<T>(name, std::move(obj));
	}

	bool findShaderFile(const path& p, path& result) const;

	static Content* instance() { return s_instance; }

private:
	using simple_registry = std::unordered_multimap<std::string, path>;
	using sub_registry = std::unordered_map<std::string, path>;
	using registry = std::unordered_map<std::type_index, sub_registry>;

	path m_contentRoot;
	registry m_registry;
	simple_registry m_genericRegistry;
	object_pool m_pool;

	std::vector<path> m_shaderIncludeDirs;

	unsigned int m_logIndentLevel;

	static Content* s_instance;


	template<typename T>
	std::string getObjectName(const nlohmann::json& json)
	{
		auto it = json.find("name");
		if (it != json.end() && it->is_string()) {
			return it->get<std::string>();
		}

		return m_pool.getNewAnonymousName<T>();
	}

	void scanContentFolder(const path& p);
	void addFile(const path& p);
};

#endif // CONTENT_HPP