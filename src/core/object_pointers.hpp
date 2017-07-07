#ifndef OBJECT_POINTERS_HPP
#define OBJECT_POINTERS_HPP

#include <memory>

class NamedObject;

struct object_destroyer
{
	void operator() (NamedObject* obj) const noexcept;
};

template<typename T>
using unique_obj_ptr = std::unique_ptr<T, object_destroyer>;

#endif // OBJECT_POINTERS_HPP