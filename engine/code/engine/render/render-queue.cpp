#include "render-queue.h"

#include "material.h"
#include "mesh.h"

#include <glad/gl.h>

void RenderQueue::Init() {
    m_frame_ubo = Buffer(Buffer::Type::Uniform, nullptr, sizeof(FrameUniforms), true);
}

void RenderQueue::Shutdown() {
    m_frame_ubo = Buffer();
}

void RenderQueue::Submit(RenderCommand command) {
    m_commands.push_back(command);
}

void RenderQueue::Draw(const FrameUniforms& uniforms) {
    m_frame_ubo.Update(&uniforms, sizeof(uniforms));
    m_frame_ubo.BindBase(FRAME_DATA_BINDING);

    for (auto& command : m_commands) {
        ASSERT(command.material);

        if (command.material && command.mesh) {
            command.material->Bind();
            command.material->GetShaderProgram()->SetUniform(MODEL_MATRIX_LOCATION, command.model_mat);

            command.mesh->Bind();
            command.mesh->Draw();
        }
    }

    m_commands.clear();
}

