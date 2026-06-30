#pragma once

#include "../graphics/shader-program.h"

#include "../graphics/texture.h"

#include <glm/mat4x4.hpp>

#include <memory>
#include <unordered_map>

struct Vec2 {
    f32 x, y;
};

constexpr u32 MODEL_MATRIX_PARAM = 0;

class Material {
public:
    static constexpr u32 PBR_TEXTURE_ALBEDO_SLOT = 0;

    explicit Material(const std::shared_ptr<ShaderProgram> &program);
    ~Material() = default;

    ShaderProgram* GetShaderProgram() const;

    void SetParam(u32 param, f32 value);
    void SetParam(u32 param, f32 v0, f32 v1);
    void SetParam(u32 param, const glm::mat4& mat);
    void SetParam(u32 texture_slot, const std::shared_ptr<Texture>& texture);

    void Bind();

    bool IsValid() const;

    static std::shared_ptr<Material> Load(std::string_view path);

private:
    std::shared_ptr<ShaderProgram> m_program;
    std::unordered_map<u32, f32> m_float_params;
    std::unordered_map<u32, Vec2> m_float2_params;
    std::unordered_map<u32, glm::mat4> m_mat4_params;
    std::unordered_map<u32, std::shared_ptr<Texture>> m_textures;
};

