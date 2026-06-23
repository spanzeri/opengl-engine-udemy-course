#pragma once

#include "../base.h"

#include "../graphics/buffer.h"

#include <glm/mat4x4.hpp>
#include <vector>

class Mesh;
class Material;

struct RenderCommand {
    Mesh* mesh = nullptr;
    Material* material = nullptr;
    glm::mat4 model_mat = glm::mat4(1.0f);
};

constexpr u32 FRAME_DATA_BINDING = 0;
constexpr u32 MODEL_MATRIX_LOCATION = 0;

struct FrameUniforms {
    glm::mat4   view;
    glm::mat4   proj;
    glm::vec4   camera_pos;
    f32         time;
    f32         pad[3];
};

class RenderQueue {
public:
    void Init();
    void Shutdown();

    void Submit(RenderCommand command);
    void Draw(const FrameUniforms& frame);

private:
    std::vector<RenderCommand> m_commands;
    Buffer m_frame_ubo;
};

