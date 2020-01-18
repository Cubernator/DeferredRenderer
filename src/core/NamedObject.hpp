#ifndef CORE_NAMEDOBJECT_HPP
#define CORE_NAMEDOBJECT_HPP

#include "Object.hpp"
#include "guid.hpp"

#include <string>
#include <type_traits>

namespace hexeract
{
	class NamedObject : public Object
	{
	public:
		NamedObject();
		virtual ~NamedObject() = 0;

		NamedObject(const NamedObject& other) = default;
		NamedObject(NamedObject&& other) = default;

		NamedObject& operator=(const NamedObject& other) = default;
		NamedObject& operator=(NamedObject&& other) = default;

		guid id() const { return m_id; }

		const std::string& name() const { return m_name; }
		void setName(const std::string& name);

	private:
		const guid m_id;
		std::string m_name;

		friend class ObjectRegistry;
	};

	template<typename T>
	using is_named_object = std::is_base_of<NamedObject, T>;
}

#define CHECK_NAMED_OBJECT_TYPE(t) static_assert(hexeract::is_named_object<t>::value, "Only types derived from NamedObject are allowed!")

#endif // CORE_NAMEDOBJECT_HPP