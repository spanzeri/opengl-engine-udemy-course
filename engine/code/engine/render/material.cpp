#include "material.h"

Material::Material(const std::shared_ptr<ShaderProgram> &program)
{
    m_program = program;
}

Material::~Material()
{
    m_program.reset();
}

ShaderProgram* Material::GetShaderProgram() const {
    return m_program.get();
}

void Material::SetParam(u32 param, f32 value)
{
    m_float_params[param] = value;
}

void Material::SetParam(u32 param, f32 v0, f32 v1) {
    m_float2_params[param] = { v0, v1 };
}

void Material::SetParam(u32 param, const glm::mat4& mat) {
    m_mat4_params[param] = mat;
}

void Material::Bind() {
    if (!m_program) {
        return;
    }

    m_program->Bind();

    for (const auto &param : m_float_params) {
        m_program->SetUniform(param.first, param.second);
    }
    for (const auto &param : m_float2_params) {
        m_program->SetUniform(param.first, param.second.x, param.second.y);
    }
    for (const auto &param : m_mat4_params) {
        m_program->SetUniform(param.first, param.second);
    }
}

bool Material::IsValid() const {
    return m_program != nullptr;
}
