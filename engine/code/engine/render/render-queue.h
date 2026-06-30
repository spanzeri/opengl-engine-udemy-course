#pragma once

#include "../common.h"
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

class RenderQueue {
public:
    void Init();
    void Shutdown();

    void Submit(RenderCommand command);
    void Draw(const FrameUniforms& frame, std::span<LightData> lights);

private:
    std::vector<RenderCommand> m_commands;
    Buffer m_frame_ubo;
};

