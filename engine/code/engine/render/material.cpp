#include "material.h"

#include "../engine.h"

#include <nlohmann/json.hpp>

Material::Material(const std::shared_ptr<ShaderProgram> &program) {
    m_program = program;
}

ShaderProgram* Material::GetShaderProgram() const {
    return m_program.get();
}

void Material::SetParam(u32 param, f32 value) {
    if (param != -1) {
        m_float_params[param] = value;
    }
}

void Material::SetParam(u32 param, f32 v0, f32 v1) {
    m_float2_params[param] = { v0, v1 };
}

void Material::SetParam(u32 param, const glm::mat4& mat) {
    m_mat4_params[param] = mat;
}

void Material::SetParam(u32 texture_slot, const std::shared_ptr<Texture>& texture) {
    m_textures[texture_slot] = texture;
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
    for (const auto &texture : m_textures) {
        m_program->SetTexture(texture.first, texture.second.get());
    }
}

bool Material::IsValid() const {
    return m_program != nullptr;
}

std::shared_ptr<Material> Material::Load(std::string_view path) {
    auto& fs = Engine::GetInstance().file_system;

    auto content = fs.LoadAssetTextFile(path);
    if (content.empty()) {
        return nullptr;
    }

    auto shader_program = std::shared_ptr<ShaderProgram>();

    auto json = nlohmann::json::parse(content);
    if (json.contains("shader")) {
        auto shader_val = json["shader"];
        std::string vert_shader_path = shader_val.value("vertex", "");
        std::string frag_shader_path = shader_val.value("fragment", "");

        auto vert_src = fs.LoadAssetTextFile(vert_shader_path);
        auto frag_src = fs.LoadAssetTextFile(frag_shader_path);

        auto& graphics = Engine::GetInstance().graphics;
        shader_program = graphics.CreateShaderProgram(vert_src, frag_src);
    }

    if (!shader_program) {
        ERROR("Failed to create shader program for material: {}", path);
        return nullptr;
    }

    auto material = std::make_shared<Material>(shader_program);

    if (json.contains("params")) {
        auto params_val = json["params"];

        if (params_val.contains("float")) {
            for (auto& p : params_val["float"]) {
                std::string name = p.value("name", "");
                u32 location = shader_program->GetUniformLocation(name);

                f32 value = p.value("value", 0.0f);
                material->SetParam(location, value);
            }
        }

        if (params_val.contains("float2")) {
            for (auto& p : params_val["float2"]) {
                std::string name = p.value("name", "");
                u32 location = shader_program->GetUniformLocation(name);

                f32 value0 = p.value("value0", 0.0f);
                f32 value1 = p.value("value1", 0.0f);
                material->SetParam(location, value0, value1);
            }
        }

        if (params_val.contains("textures")) {
            for (auto& p : params_val["textures"]) {
                u32 slot = p.value("slot", 0u);
                std::string path = p.value("path", "");
                if (!path.empty()) {
                    auto tex = Engine::GetInstance().texture_manager.GetOrLoadTexture(path);
                    material->SetParam(slot, MOV(tex));
                }
            }
        }
    }

    return material;
}

