#include "object_pointers.hpp"
#include "ObjectRegistry.hpp"

namespace hexeract
{
	void object_destroyer::operator()(NamedObject* obj) const noexcept
	{
		instance<ObjectRegistry>()->destroy(obj);
	}
}
