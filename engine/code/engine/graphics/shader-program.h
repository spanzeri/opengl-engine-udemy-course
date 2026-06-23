#pragma once

#include "../base.h"

#include <glm/mat4x4.hpp>

class Texture;

class ShaderProgram : NonCopiable {
public:
    explicit ShaderProgram(u32 program_id);
    ~ShaderProgram();

    u32 GetID() const;
    s32 GetUniformLocation(const std::string& name) const;

    void Bind();
    void SetUniform(s32 location, f32 value);
    void SetUniform(s32 location, f32 v0, f32 v1);
    void SetUniform(s32 location, const glm::mat4& mat);
    void SetTexture(s32 location, Texture* texture);

    bool IsValid() const;

private:
    u32 m_id = 0;
};
