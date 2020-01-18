#ifndef CORE_COMPONENTMODULE_HPP
#define CORE_COMPONENTMODULE_HPP

namespace hexeract
{
	class Component;

	class ComponentModule
	{
	public:
		virtual void addComponent(Component* cmpt) = 0;
		virtual void removeComponent(Component* cmpt) = 0;
	};
}

#endif // CORE_COMPONENTMODULE_HPP