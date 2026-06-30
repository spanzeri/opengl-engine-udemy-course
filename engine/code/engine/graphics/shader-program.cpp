#include "shader-program.h"
#include "engine/graphics/texture.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(GLuint program_id) : m_id(program_id) {}

ShaderProgram::~ShaderProgram() {
    if (m_id) {
        glDeleteProgram(m_id);
    }
}

u32 ShaderProgram::GetID() const {
    return m_id;
}

s32 ShaderProgram::GetUniformLocation(const std::string& name) const {
    return glGetUniformLocation(m_id, name.c_str());
}

void ShaderProgram::Bind() {
    glUseProgram(m_id);
}

void ShaderProgram::SetUniform(s32 location, f32 value) {
    if (m_id != 0 && location != -1) {
        glProgramUniform1f(m_id, location, value);
    }
}

void ShaderProgram::SetUniform(s32 location, f32 v0, f32 v1) {
    if (m_id != 0 && location != -1) {
        glProgramUniform2f(m_id, location, v0, v1);
    }
}

void ShaderProgram::SetUniform(s32 location, glm::vec3 v3) {
    if (m_id != 0 && location != -1) {
        glProgramUniform3f(m_id, location, v3.x, v3.y, v3.z);
    }
}

void ShaderProgram::SetUniform(s32 location, const glm::mat4& mat) {
    if (m_id != 0 && location != -1) {
        glProgramUniformMatrix4fv(m_id, location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void ShaderProgram::SetTexture(s32 slot, Texture* texture) {
    if (texture) {
        glBindTextureUnit(slot, texture->GetId());
    }
}

bool ShaderProgram::IsValid() const {
    return m_id != 0;
}

