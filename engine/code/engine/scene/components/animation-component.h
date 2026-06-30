#pragma once

#include "../component.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct KeyFrameVec3 {
    f32 time = 0.0f;
    glm::vec3 value = glm::vec3(0);
};

struct KeyFrameQuat {
    f32 time = 0.0f;
    glm::quat value = glm::quat(1, glm::vec3(0));
};

struct TransformTrack {
    std::string target_name;
    std::vector<KeyFrameVec3> positions;
    std::vector<KeyFrameQuat> rotations;
    std::vector<KeyFrameVec3> scales;
};

struct AnimationClip {
    std::string name;
    f32 duration = 0.0f;
    bool looping = false;
    std::vector<TransformTrack> tracks;
};

struct ObjectBinding {
    GameObject* object = nullptr;
    std::vector<usize> track_indices;
};

class AnimationComponent : public Component {
    COMPONENT(AnimationComponent);

public:
    void Update(f32 dt) override;

    void SetClip(AnimationClip* clip);
    void RegisterClip(std::string_view name, const std::shared_ptr<AnimationClip>& clip);
    void Play(std::string_view name, bool loop = true);

private:
    void BuildBindings();
    glm::vec3 Interpolate(std::span<KeyFrameVec3> keys, f32 time) const;
    glm::quat Interpolate(std::span<KeyFrameQuat> keys, f32 time) const;

private:
    AnimationClip* m_clip       = nullptr;
    f32            m_time       = 0.0f;
    bool           m_looping    = true;
    bool           m_is_playing = false;

    std::unordered_map<std::string, std::shared_ptr<AnimationClip>> m_clips;
    std::unordered_map<GameObject*, ObjectBinding> m_bindings;
};

