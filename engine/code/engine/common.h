#pragma once

#include "base.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

constexpr u32 FRAME_DATA_BINDING = 0;
constexpr u32 MODEL_MATRIX_LOCATION = 0;

struct FrameUniforms {
    glm::mat4   view;
    glm::mat4   proj;
    glm::vec4   camera_pos;
    f32         time;
    f32         pad[3];
};

struct LightData {
    glm::vec3 color;
    glm::vec3 position;
};

