#include "player-controller-component.h"

#include "../../engine.h"
#include "../../input-manager.h"

#include <glm/trigonometric.hpp>
#include <glm/gtx/euler_angles.hpp>

constexpr f32 PITCH_LIMIT = glm::radians(80.f);

void PlayerControllerComponent::Update(f32 dt) {
    auto& input = Engine::GetInstance().input;

    auto rotation = m_owner->GetRotation();
    if (input.IsButtonDown(Button::Right)) {
        auto mouse_delta = input.GetMouseDeltaPosition();
        rotation.x -= mouse_delta.y * m_sensitivity * dt;
        rotation.y -= mouse_delta.x * m_sensitivity * dt;

        rotation.x = Clamp(rotation.x, -PITCH_LIMIT, PITCH_LIMIT);

        if (rotation.y < -M_PI) {
            rotation.y += M_PI * 2.0f;
        } else if (rotation.y > M_PI) {
            rotation.y -= M_PI * 2.0f;
        }

        m_owner->SetRotation(rotation);
    }

    auto rot = glm::eulerAngleZYX(rotation.z, rotation.y, rotation.x);
    glm::vec3 front = -rot[2];
    glm::vec3 right = rot[0];

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

