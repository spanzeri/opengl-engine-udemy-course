#include "player-controller-component.h"

#include "../../engine.h"
#include "../../input-manager.h"

#include <glm/trigonometric.hpp>
#include <glm/gtx/euler_angles.hpp>

constexpr f32 PITCH_LIMIT = glm::radians(80.f);

void PlayerControllerComponent::Update(f32 dt) {
    auto& input = Engine::GetInstance().input;

    if (input.IsButtonDown(Button::Right)) {
        auto mouse_delta = input.GetMouseDeltaPosition();
        m_yaw   -= mouse_delta.x * m_sensitivity * dt;
        m_pitch -= mouse_delta.y * m_sensitivity * dt;

        m_pitch = Clamp(m_pitch, -PITCH_LIMIT, PITCH_LIMIT);

        if      (m_yaw < -M_PI) { m_yaw += M_PI * 2.0f; }
        else if (m_yaw >  M_PI) { m_yaw -= M_PI * 2.0f; }

        m_owner->SetRotation(glm::normalize(
            glm::angleAxis(m_yaw, glm::vec3(0, 1, 0)) *
            glm::angleAxis(m_pitch, glm::vec3(1, 0, 0))
        ));
    }

    glm::vec3 front = m_owner->GetRotation() * glm::vec3(0, 0, -1);
    glm::vec3 right = m_owner->GetRotation() * glm::vec3(1, 0, 0);

    auto move = glm::vec2(0);
    if (input.IsKeyDown(Key::W)) { move.y += 1.0f; }
    if (input.IsKeyDown(Key::S)) { move.y -= 1.0f; }
    if (input.IsKeyDown(Key::D)) { move.x += 1.0f; }
    if (input.IsKeyDown(Key::A)) { move.x -= 1.0f; }

    if (move.x != 0 || move.y != 0) {
        move = glm::normalize(move);
        m_owner->SetPosition(m_owner->GetPosition() + (move.x * right + move.y * front) * m_move_speed * dt);
    }
}

