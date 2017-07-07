#ifndef OBJECT_HPP
#define OBJECT_HPP

class Object
{
public:
	virtual ~Object() = 0;
};

void destroy_object(Object* obj);

#endif // OBJECT_HPP