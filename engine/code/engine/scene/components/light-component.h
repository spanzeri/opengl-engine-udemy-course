#pragma once

#include "../component.h"

#include <glm/vec3.hpp>

class LightComponent : public Component {
    COMPONENT(LightComponent);

public:
    LightComponent() = default;
    explicit LightComponent(glm::vec3 color) : color(color) {}

    void Update(f32 dt) override;

    glm::vec3 color = glm::vec3(1);
};

