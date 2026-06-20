#include "component.h"

#include "game-object.h"

u64 Component::next_component_id = 0;

GameObject* Component::GetOwner() const {
    return m_owner;
}

