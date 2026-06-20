#pragma once

#include "../component.h"

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

class CameraComponent : public Component {
    COMPONENT(CameraComponent);

public:
    void Update(f32 dt) override;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(f32 aspect) const;

    f32 fov  = glm::radians(60.0f);
    f32 near = 0.1f;
    f32 far  = 1'000.0f;
};
