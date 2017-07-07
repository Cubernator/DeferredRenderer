#ifndef COMPONENTMODULE_HPP
#define COMPONENTMODULE_HPP

class Component;

class ComponentModule
{
public:
	virtual void addComponent(Component* cmpt) = 0;
	virtual void removeComponent(Component* cmpt) = 0;
};

#endif // COMPONENTMODULE_HPP