#include "Object.hpp"
#include "ObjectRegistry.hpp"
#include "Entity.hpp"
#include "Component.hpp"
#include "Scene.hpp"

Object::~Object() { }

void destroy_object(Object* obj)
{
	auto nobj = dynamic_cast<NamedObject*>(obj);
	if (nobj) {
		auto entity = dynamic_cast<Entity*>(nobj);
		if (entity) {
			auto ps = entity->parentScene();
			if (ps) {
				ps->removeEntity(entity);
			}
		}

		instance<ObjectRegistry>()->destroy(nobj);
	}

	auto cmpt = dynamic_cast<Component*>(obj);
	if (cmpt) {
		destroy_component(cmpt);
	}
}
