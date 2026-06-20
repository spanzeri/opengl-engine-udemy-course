#pragma once

#include "../component.h"

class PlayerControllerComponent : public Component {
    COMPONENT(PlayerControllerComponent);

public:
    void Update(f32 dt) override;

private:
    f32 m_sensitivity = 0.1f;
    f32 m_move_speed = 1.0f;
};

