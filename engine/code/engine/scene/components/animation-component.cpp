#include "animation-component.h"

#include "../game-object.h"

void AnimationComponent::Update(f32 dt) {
    if (!m_clip || !m_is_playing) {
        return;
    }

    m_time += dt;

    if (m_time > m_clip->duration) {
        if (m_looping) {
            m_time = fmodf(m_time, m_clip->duration);
        } else {
            m_time = 0.0f;
            m_is_playing = false;
            return;
        }
    }

    for (auto& binding : m_bindings) {
        auto& obj = binding.first;
        const auto& track_indices = binding.second.track_indices;

        for (auto i : track_indices) {
            auto& track = m_clip->tracks[i];

            if (!track.positions.empty()) {
                auto position = Interpolate(track.positions, m_time);
                obj->SetPosition(position);
            }
            if (!track.rotations.empty()) {
                auto rotation = Interpolate(track.rotations, m_time);
                obj->SetRotation(rotation);
            }
            if (!track.scales.empty()) {
                auto scale = Interpolate(track.scales, m_time);
                obj->SetScale(scale);
            }
        }
    }
}

void AnimationComponent::SetClip(AnimationClip* clip) {
    m_clip = clip;
    BuildBindings();
}

void AnimationComponent::RegisterClip(std::string_view name, const std::shared_ptr<AnimationClip>& clip) {
    m_clips[std::string(name)] = clip;
}

void AnimationComponent::Play(std::string_view name, bool loop) {
    if (m_clip && m_clip->name == name) {
        // This was already our last active clip... replay
    } else if (auto it = m_clips.find(std::string(name)); it != m_clips.end()) {
        ASSERT(it->first == it->second->name);
        SetClip(it->second.get());
    }

    if (m_clip) {
        m_time = 0.0f;
        m_is_playing = true;
        m_looping = loop;
    }
}

void AnimationComponent::BuildBindings() {
    m_bindings.clear();

    for (usize i = 0; i < m_clip->tracks.size(); i++) {
        const auto& track = m_clip->tracks[i];
        auto target_object = m_owner->FindChildByName(track.target_name);
        if (target_object) {
            if (auto it = m_bindings.find(target_object); it != m_bindings.end()) {
                it->second.track_indices.push_back(i);
            } else {
                auto binding = ObjectBinding{ .object = target_object, .track_indices = { i } };
                m_bindings.emplace(target_object, MOV(binding));
            }
        }
    }
}

glm::vec3 AnimationComponent::Interpolate(std::span<KeyFrameVec3> keys, f32 time) const {
    if (keys.empty()) {
        return glm::vec3(0);
    }
    if (keys.size() == 1) {
        return keys[0].value;
    }
    if (time <= keys.front().time) {
        return keys.front().value;
    }
    if (time >= keys.back().time) {
        return keys.back().value;
    }

    usize i0 = 0, i1 = 0;
    for (usize i = 0; i < keys.size(); i++) {
        if (time <= keys[i].time) {
            i1 = i;
            break;
        }
    }

    i0 = i1 > 0 ? i1 - 1 : 0;

    if (time >= keys[i0].time && time <= keys[i1].time) {
        f32 dt = keys[i1].time - keys[i0].time;
        f32 t  = (time - keys[i0].time) / dt;

        return Lerp(keys[i0].value, keys[i1].value, t);
    }

    return keys.back().value;
}

glm::quat AnimationComponent::Interpolate(std::span<KeyFrameQuat> keys, f32 time) const {
    if (keys.empty()) {
        return glm::quat(1, 0, 0, 0);
    }
    if (keys.size() == 1) {
        return keys[0].value;
    }
    if (time <= keys.front().time) {
        return keys.front().value;
    }
    if (time >= keys.back().time) {
        return keys.back().value;
    }

    usize i0 = 0, i1 = 0;
    for (usize i = 0; i < keys.size(); i++) {
        if (time <= keys[i].time) {
            i1 = i;
            break;
        }
    }

    i0 = i1 > 0 ? i1 - 1 : 0;

    if (time >= keys[i0].time && time <= keys[i1].time) {
        f32 dt = keys[i1].time - keys[i0].time;
        f32 t  = (time - keys[i0].time) / dt;

        return glm::slerp(keys[i0].value, keys[i1].value, t);
    }

    return keys.back().value;
}

