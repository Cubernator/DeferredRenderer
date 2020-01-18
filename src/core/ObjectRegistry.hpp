#ifndef CORE_OBJECTREGISTRY_HPP
#define CORE_OBJECTREGISTRY_HPP

#include "NamedObject.hpp"
#include "object_pointers.hpp"
#include "util/singleton.hpp"

#include <vector>

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/identity.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/composite_key.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/global_fun.hpp"

#include "boost/iterator/transform_iterator.hpp"

namespace hexeract
{
	namespace mi = boost::multi_index;

	class ObjectRegistry : public singleton<ObjectRegistry>
	{
	public:
		template<typename T>
		using tobj_array = std::vector<T*>;

		using obj_array = tobj_array<NamedObject>;

		ObjectRegistry();

		template<typename T>
		unique_obj_ptr<T> addUnique(std::unique_ptr<T> obj)
		{
			CHECK_NAMED_OBJECT_TYPE(T);

			T* tmp = obj.get();
			if (addInt(std::move(obj))) {
				return unique_obj_ptr<T>(tmp);
			}
			return nullptr;
		}

		template<typename T>
		T* add(std::unique_ptr<T> obj)
		{
			return addUnique<T>(std::move(obj)).release();
		}

		template<typename T, typename... Args>
		unique_obj_ptr<T> emplaceUnique(const std::string& name, Args&&... args)
		{
			CHECK_NAMED_OBJECT_TYPE(T);

			auto obj = std::make_unique<T>(std::forward<Args>(args)...);
			obj->setName(name);
			return addUnique<T>(std::move(obj));
		}

		template<typename T, typename... Args>
		T* emplace(const std::string& name, Args&&... args)
		{
			return emplaceUnique<T>(name, std::forward<Args>(args)...).release();
		}

		void destroy(NamedObject* obj);

		NamedObject* getById(guid id);

		obj_array findNAll(const std::string& name);
		NamedObject* findNFirst(const std::string& name);

		obj_array findTAll(std::type_index tid);
		obj_array findTAll(const std::string& typeName);
		NamedObject* findTFirst(std::type_index tid);
		NamedObject* findTFirst(const std::string& typeName);

		obj_array findTNAll(std::type_index tid, const std::string& name);
		obj_array findTNAll(const std::string& typeName, const std::string& name);
		NamedObject* findTNFirst(std::type_index tid, const std::string& name);
		NamedObject* findTNFirst(const std::string& typeName, const std::string& name);

		template<typename T>
		tobj_array<T> findTAll()
		{
			return findTAllInt<T>(typeid(T));
		}

		template<typename T>
		T* findTFirst()
		{
			return static_cast<T*>(findTFirst(typeid(T)));
		}

		template<typename T>
		tobj_array<T> findTNAll(const std::string& name)
		{
			return findTNAllInt<T>(typeid(T), name);
		}

		template<typename T>
		T* findTNFirst(const std::string& name)
		{
			return static_cast<T*>(findTNFirst(typeid(T), name));
		}

		guid getGUID()
		{
			return m_nextID++;
		}

	private:
		inline friend std::type_index get_obj_typeid(const NamedObject& obj)
		{
			return typeid(obj);
		}

		using obj_ptr = std::unique_ptr<NamedObject>;

		struct by_id { };
		struct by_type { };
		struct by_name { };
		struct by_type_and_name { };

		using key_type = mi::global_fun<const NamedObject&, std::type_index, &get_obj_typeid>;
		using key_name = mi::member<NamedObject, std::string, &NamedObject::m_name>;

		struct obj_container_indices : public mi::indexed_by<
			mi::hashed_unique<mi::tag<by_id>, mi::member<NamedObject, const guid, &NamedObject::m_id>>,
			mi::hashed_non_unique<mi::tag<by_type>, key_type>,
			mi::hashed_non_unique<mi::tag<by_name>, key_name>,
			mi::hashed_non_unique<mi::tag<by_type_and_name>, mi::composite_key<NamedObject, key_type, key_name>>
		> { };

		using obj_container = mi::multi_index_container<
			obj_ptr,
			obj_container_indices
		>;

		obj_container m_objects;
		guid m_nextID;

		void setObjectName(NamedObject* obj, const std::string& name);

		bool addInt(obj_ptr obj);

		template<typename T>
		tobj_array<T> findTAllInt(std::type_index tid)
		{
			const auto& idx = m_objects.get<by_type>();
			auto range = idx.equal_range(tid);
			auto tl = [](const obj_ptr& obj) {
				return static_cast<T*>(obj.get());
			};
			return tobj_array<T>(boost::make_transform_iterator(range.first, tl), boost::make_transform_iterator(range.second, tl));
		}

		template<typename T>
		tobj_array<T> findTNAllInt(std::type_index tid, const std::string& name)
		{
			const auto& idx = m_objects.get<by_type_and_name>();
			auto range = idx.equal_range(std::make_tuple(tid, name));
			auto tl = [](const obj_ptr& obj) {
				return static_cast<T*>(obj.get());
			};
			return tobj_array<T>(boost::make_transform_iterator(range.first, tl), boost::make_transform_iterator(range.second, tl));
		}

		friend class NamedObject;
	};
}

#endif // OBJECTREGISTRY_HPP