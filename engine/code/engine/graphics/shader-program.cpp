#include "shader-program.h"

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

void ShaderProgram::Bind() {
    glUseProgram(m_id);
}

void ShaderProgram::SetUniform(u32 param, f32 value) {
    if (m_id != 0) {
        glProgramUniform1f(m_id, param, value);
    }
}

void ShaderProgram::SetUniform(u32 param, f32 v0, f32 v1) {
    if (m_id != 0) {
        glProgramUniform2f(m_id, param, v0, v1);
    }
}

void ShaderProgram::SetUniform(u32 param, const glm::mat4& mat) {
    if (m_id != 0) {
        glProgramUniformMatrix4fv(m_id, param, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

bool ShaderProgram::IsValid() const {
    return m_id != 0;
}

