#include "ImageEffect.hpp"
#include "core/Entity.hpp"
#include "core/component_registry.hpp"
#include "scripting/class_registry.hpp"

REGISTER_ABSTRACT_COMPONENT_CLASS(ImageEffect)

SCRIPTING_REGISTER_DERIVED_CLASS(ImageEffect, Component)
