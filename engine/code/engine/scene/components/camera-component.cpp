#include "camera-component.h"

#include "../game-object.h"

#include <glm/gtc/matrix_transform.hpp>

void CameraComponent::Update(f32 dt) {
    Unused(dt);
}

glm::mat4 CameraComponent::GetViewMatrix() const {
    auto rot = m_owner->GetRotation();
    auto pos = m_owner->GetPosition();

    if (auto* parent = m_owner->GetParent(); parent) {
        auto parent_rot = parent->GetWorldRotation();
        rot = parent_rot * rot;
        pos = parent->GetWorldPosition() + parent_rot * pos;
    }

    auto m = glm::translate(glm::mat4_cast(glm::conjugate(rot)), -pos);
    return m;
}

glm::mat4 CameraComponent::GetProjectionMatrix(f32 aspect) const {
    return glm::perspective(fov, aspect, near, far);
}

