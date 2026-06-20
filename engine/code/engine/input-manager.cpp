#include "input-manager.h"

void InputManager::StartFrame() {
    for (auto& key : m_keys) {
        key.transitions = 0;
    }
    for (auto& button : m_buttons) {
        button.transitions = 0;
    }
    m_prev_mouse_pos = m_mouse_pos;
    m_mouse_pos_delta = glm::vec2(0);
    m_mouse_scroll = 0;
}

void InputManager::SetKeyPressed(Key key, bool pressed) {
    bool was_pressed = (m_keys[(u32)key].is_down != 0);
    if (was_pressed != pressed) {
        m_keys[(u32)key].transitions += 1;
        m_keys[(u32)key].is_down = pressed ? 1 : 0;
    }
}

void InputManager::SetButtonPressed(Button button, bool pressed) {
    bool was_pressed = (m_buttons[(u32)button].is_down != 0);
    if (was_pressed != pressed) {
        m_buttons[(u32)button].transitions += 1;
        m_buttons[(u32)button].is_down = pressed ? 1 : 0;
    }
}

void InputManager::SetMousePosition(f32 x, f32 y) {
    m_mouse_pos = glm::vec2(x, y);
    m_mouse_pos_delta = m_mouse_pos - m_prev_mouse_pos;
}

void InputManager::SetMouseScroll(f32 s) {
    m_mouse_scroll = s;
}

bool InputManager::IsKeyJustPressed(Key key) const {
    return m_keys[(u32)key].is_down != 0 && m_keys[(u32)key].transitions > 0;
}

bool InputManager::IsKeyJustReleased(Key key) const {
    return m_keys[(u32)key].is_down == 0 && m_keys[(u32)key].transitions > 0;
}

bool InputManager::IsKeyDown(Key key) const {
    return m_keys[(u32)key].is_down != 0;
}

bool InputManager::IsKeyUp(Key key) const {
    return m_keys[(u32)key].is_down == 0;
}

bool InputManager::IsButtonJustPressed(Button button) const {
    return m_buttons[(u32)button].is_down != 0 && m_buttons[(u32)button].transitions > 0;
}

bool InputManager::IsButtonJustReleased(Button button) const {
    return m_buttons[(u32)button].is_down == 0 && m_buttons[(u32)button].transitions > 0;
}

bool InputManager::IsButtonDown(Button button) const {
    return m_buttons[(u32)button].is_down != 0;
}

bool InputManager::IsButtonUp(Button button) const {
    return m_buttons[(u32)button].is_down == 0;
}

glm::vec2 InputManager::GetMousePosition() const {
    return m_mouse_pos;
}

glm::vec2 InputManager::GetMouseDeltaPosition() const {
    return m_mouse_pos_delta;
}

f32 InputManager::GetMouseScroll() const {
    return m_mouse_scroll;
}

