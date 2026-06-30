#pragma once

#include "shader-program.h"

#include <memory>

class ShaderProgram;

class GraphicsAPI : NonCopiable {
public:
    GraphicsAPI() = default;

    void Init();
    void Shutdown();

    std::shared_ptr<ShaderProgram> CreateShaderProgram(std::string_view vertex_source, std::string_view fragment_source) const;
    std::shared_ptr<ShaderProgram> GetDefaultShaderProgram() const;

    void SetClearColor(f32 r, f32 g, f32 b, f32 a) const;
    void ClearBuffers() const;

private:
    static std::shared_ptr<ShaderProgram> s_default_shader_program;
};

