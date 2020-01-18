#ifndef CORE_TYPE_REGISTRY_HPP
#define CORE_TYPE_REGISTRY_HPP

#include <typeindex>
#include <exception>
#include <string>

#include "core/NamedObject.hpp"
#include "util/import.hpp"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/member.hpp"

namespace hexeract
{
	namespace mi = boost::multi_index;

	struct object_importer
	{
		virtual std::unique_ptr<NamedObject> importFromFile(const path& p) const = 0;
		virtual std::unique_ptr<NamedObject> importFromJson(const nlohmann::json& json) const = 0;
	};

	struct object_type
	{
		std::type_index id;
		std::string name;
		std::string extension;
		std::shared_ptr<object_importer> importer;

		object_type();
		object_type(std::type_index id, const std::string& name, const std::string& extension, std::shared_ptr<object_importer> importer);

		operator bool() const;
	};

	class type_registry
	{
	public:
		static void registerObjectType(object_type type);

		static const object_type& findByName(const std::string& name);
		static const object_type& findById(std::type_index id);
		static const object_type& findByExtension(const std::string& ext);

		static const object_type& getType(const NamedObject* obj);

		template<typename T>
		static const object_type& get()
		{
			return findById(typeid(T));
		}

		struct by_id { };
		struct by_name { };
		struct by_extension { };

		struct type_container_indices : public mi::indexed_by<
			mi::hashed_unique<mi::tag<by_name>, mi::member<object_type, std::string, &object_type::name>>,
			mi::hashed_unique<mi::tag<by_id>, mi::member<object_type, std::type_index, &object_type::id>>,
			mi::hashed_non_unique<mi::tag<by_extension>, mi::member<object_type, std::string, &object_type::extension>>
		> { };

		using type_container = mi::multi_index_container<
			object_type,
			type_container_indices
		>;

	private:
		static type_container& s_types;
		static object_type s_empty_type;

		friend struct type_registry_initializer;
	};

	inline std::type_index type_name_to_id(const std::string& typeName)
	{
		return type_registry::findByName(typeName).id;
	}

	template<typename T>
	std::unique_ptr<T> import_object(const object_type& t, const path& p)
	{
		return import_object<T>(p);
	}

	template<>
	inline std::unique_ptr<NamedObject> import_object<NamedObject>(const object_type& t, const path& p)
	{
		return t.importer->importFromFile(p);
	}

	template<typename T>
	std::unique_ptr<T> json_to_object(const object_type& t, const nlohmann::json& json)
	{
		return json_to_object<T>(json);
	}

	template<>
	inline std::unique_ptr<NamedObject> json_to_object<NamedObject>(const object_type& t, const nlohmann::json& json)
	{
		return t.importer->importFromJson(json);
	}

	namespace detail
	{
		template<typename T, typename Enable = void>
		struct type_importer_t : public object_importer
		{
			virtual std::unique_ptr<NamedObject> importFromFile(const path& p) const final
			{
				return import_object<T>(p);
			}

			virtual std::unique_ptr<NamedObject> importFromJson(const nlohmann::json& json) const final
			{
				return json_to_object<T>(json);
			}
		};

		template<typename T>
		struct type_importer_t<T, std::enable_if_t<!is_json_initializable<T>::value>> : public object_importer
		{
			virtual std::unique_ptr<NamedObject> importFromFile(const path& p) const final
			{
				return import_object<T>(p);
			}

			virtual std::unique_ptr<NamedObject> importFromJson(const nlohmann::json& json) const final
			{
				throw std::runtime_error("Attempted to json-initialize an object of incompatible type!");
			}
		};

		template<typename T>
		struct type_registerer
		{
			CHECK_NAMED_OBJECT_TYPE(T);

			type_registerer(const std::string& name, const std::string& extension)
			{
				type_registry::registerObjectType(object_type(typeid(T), name, extension, std::make_shared<type_importer_t<T>>()));
			}
		};
	}

	static struct type_registry_initializer
	{
		type_registry_initializer();
		~type_registry_initializer();
	} type_registry_init;
}

#define REGISTER_OBJECT_TYPE(c, e)							\
namespace													\
{															\
	hexeract::detail::type_registerer<c> register_type_##c (#c, e);	\
}

#define REGISTER_OBJECT_TYPE_NO_EXT(c) REGISTER_OBJECT_TYPE(c, "")

#endif // CORE_TYPE_REGISTRY_HPP