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

void RenderQueue::Draw(const FrameUniforms& uniforms, std::span<LightData> lights) {
    m_frame_ubo.Update(&uniforms, sizeof(uniforms));
    m_frame_ubo.BindBase(FRAME_DATA_BINDING);

    for (auto& command : m_commands) {
        ASSERT(command.material);

        if (command.material && command.mesh) {
            command.material->Bind();
            command.material->GetShaderProgram()->SetUniform(MODEL_MATRIX_LOCATION, command.model_mat);

            if (!lights.empty()) {
                auto* prog = command.material->GetShaderProgram();
                auto& light = lights[0];

                prog->SetUniform(prog->GetUniformLocation("uLight.color"), light.color);
                prog->SetUniform(prog->GetUniformLocation("uLight.position"), light.position);
            }

            command.mesh->Bind();
            command.mesh->Draw();
        }
    }

    m_commands.clear();
}

