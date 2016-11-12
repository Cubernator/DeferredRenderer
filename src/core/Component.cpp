#include "core/Component.hpp"

Component::Component(Entity* parent) : m_parent(parent) { }
Component::~Component() { }
