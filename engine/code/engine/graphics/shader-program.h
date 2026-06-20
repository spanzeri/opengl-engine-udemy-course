#pragma once

#include "../base.h"

#include <glm/mat4x4.hpp>

class ShaderProgram : NonCopiable {
public:
    explicit ShaderProgram(u32 program_id);
    ~ShaderProgram();

    u32  GetID() const;

    void Bind();
    void SetUniform(u32 param, f32 value);
    void SetUniform(u32 param, f32 v0, f32 v1);
    void SetUniform(u32 param, const glm::mat4& mat);

    bool IsValid() const;

private:
    u32 m_id = 0;
};
