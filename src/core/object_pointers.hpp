#ifndef CORE_OBJECT_POINTERS_HPP
#define CORE_OBJECT_POINTERS_HPP

#include <memory>

namespace hexeract
{
	class NamedObject;

	struct object_destroyer
	{
		void operator() (NamedObject* obj) const noexcept;
	};

	template<typename T>
	using unique_obj_ptr = std::unique_ptr<T, object_destroyer>;
}

#endif // CORE_OBJECT_POINTERS_HPP