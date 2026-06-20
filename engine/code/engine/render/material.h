#pragma once

#include "../graphics/shader-program.h"

#include <glm/mat4x4.hpp>

#include <memory>
#include <unordered_map>

struct Vec2 {
    f32 x, y;
};

constexpr u32 MODEL_MATRIX_PARAM = 0;
constexpr u32 VIEW_MATRIX_PARAM = 1;
constexpr u32 PROJECTION_MATRIX_PARAM = 2;

class Material {
public:
    explicit Material(const std::shared_ptr<ShaderProgram> &program);
    ~Material();

    ShaderProgram* GetShaderProgram() const;

    void SetParam(u32 param, f32 value);
    void SetParam(u32 param, f32 v0, f32 v1);
    void SetParam(u32 param, const glm::mat4& mat);
    void Bind();

    bool IsValid() const;

private:
    std::shared_ptr<ShaderProgram> m_program;
    std::unordered_map<u32, f32> m_float_params;
    std::unordered_map<u32, Vec2> m_float2_params;
    std::unordered_map<u32, glm::mat4> m_mat4_params;
};

