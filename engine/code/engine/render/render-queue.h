#pragma once

#include <glm/mat4x4.hpp>

#include <vector>

class Mesh;
class Material;
class GraphicsAPI;

struct RenderCommand {
    Mesh* mesh = nullptr;
    Material* material = nullptr;
    glm::mat4 model_mat = glm::mat4(1.0f);
};

struct CameraData {
    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 proj_matrix = glm::mat4(1.0f);
};

class RenderQueue {
public:
    void Submit(RenderCommand command);
    void Draw(GraphicsAPI& graphics, const CameraData& camera_data);

private:
    std::vector<RenderCommand> m_commands;
};
