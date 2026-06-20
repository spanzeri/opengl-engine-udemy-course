#include "render-queue.h"

#include "material.h"
#include "../graphics/graphics-api.h"

void RenderQueue::Submit(RenderCommand command)
{
    m_commands.push_back(command);
}

void RenderQueue::Draw(GraphicsAPI& graphics, const CameraData& camera_data)
{
    u32 last_program_id = 0;

    for (auto& command : m_commands) {
        ASSERT(command.material);

        auto* program = command.material->GetShaderProgram();
        u32 id = program->GetID();
        if (id != last_program_id) {
            last_program_id = id;
            program->SetUniform(VIEW_MATRIX_PARAM, camera_data.view_matrix);
            program->SetUniform(PROJECTION_MATRIX_PARAM, camera_data.proj_matrix);
        }

        command.material->SetParam(MODEL_MATRIX_PARAM, command.model_mat);
        command.material->SetParam(MODEL_MATRIX_PARAM, command.model_mat);

        graphics.BindMaterial(command.material);
        graphics.BindMesh(command.mesh);
        graphics.DrawMesh(command.mesh);
    }

    m_commands.clear();
}

