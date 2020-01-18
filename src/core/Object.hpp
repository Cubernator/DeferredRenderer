#ifndef CORE_OBJECT_HPP
#define CORE_OBJECT_HPP

namespace hexeract
{
	class Object
	{
	public:
		virtual ~Object() = 0;
	};

	void destroy_object(Object* obj);
}

#endif // CORE_OBJECT_HPP