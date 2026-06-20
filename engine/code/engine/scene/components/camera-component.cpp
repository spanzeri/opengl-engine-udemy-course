#include "camera-component.h"

#include "../game-object.h"

#include <glm/gtc/matrix_transform.hpp>

void CameraComponent::Update(f32 dt) {
    unused(dt);
}

glm::mat4 CameraComponent::GetViewMatrix() const {
    return glm::inverse(m_owner->GetTransformWorld());
}

glm::mat4 CameraComponent::GetProjectionMatrix(f32 aspect) const {
    return glm::perspective(fov, aspect, near, far);
}

