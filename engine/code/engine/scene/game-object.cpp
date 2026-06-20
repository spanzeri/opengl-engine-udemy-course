#include "game-object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

void GameObject::DoUpdate(f32 dt) {
    // update ourself (derived)
    Update(dt);

    // update our components
    for (const auto& component : m_components)  {
        component->Update(dt);
    }

    // update our children (and they'll update their components)
    for (auto it = m_children.begin(); it != m_children.end();) {
        if ((*it)->IsAlive()) {
            (*it)->DoUpdate(dt);
            it++;
        } else {
            it = m_children.erase(it);
        }
    }
}

bool GameObject::IsAlive() const {
    return m_is_alive;
}

void GameObject::MarkForDestroy() {
    m_is_alive = false;
}

void GameObject::AddComponent(Component* component) {
    m_components.emplace_back(component);
    component->m_owner = this;
}

glm::vec3 GameObject::GetPosition() const {
    return m_position;
}

void GameObject::SetPosition(glm::vec3 pos) {
    m_position = pos;
}

glm::vec3 GameObject::GetRotation() const  {
    return m_rotation;
}

void GameObject::SetRotation(glm::vec3 rot) {
    m_rotation = rot;
}

glm::vec3 GameObject::GetScale() const {
    return m_scale;
}

void GameObject::SetScale(glm::vec3 scale) {
    m_scale = scale;
}

glm::mat4 GameObject::GetTransformLocal() const {
    glm::mat4 m = glm::eulerAngleZYX(m_rotation.z, m_rotation.y, m_rotation.x);
    m[0] *= m_scale.x;
    m[1] *= m_scale.y;
    m[2] *= m_scale.z;
    m[3]  = glm::vec4(m_position, 1.0f);
    return m;
}

glm::mat4 GameObject::GetTransformWorld() const {
    if (!parent) {
        return GetTransformLocal();
    }

    return parent->GetTransformWorld() * GetTransformLocal();
}

