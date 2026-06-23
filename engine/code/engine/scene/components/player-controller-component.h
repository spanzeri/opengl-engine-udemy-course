#pragma once

#include "../component.h"

class PlayerControllerComponent : public Component {
    COMPONENT(PlayerControllerComponent);

public:
    void Update(f32 dt) override;

private:
    f32 m_sensitivity = 0.1f;
    f32 m_move_speed = 1.0f;

    f32 m_yaw = 0.0f;
    f32 m_pitch = 0.0f;
};

