#pragma once

#include "../base.h"

#include <memory>

class ShaderProgram;
class Material;
class Mesh;

class GraphicsAPI : NonCopiable {
public:
    GraphicsAPI() = default;

    std::shared_ptr<ShaderProgram> CreateShaderProgram(std::string_view vertex_source, std::string_view fragment_source);

    void Init();
    void Shutdown();

    void SetClearColor(f32 r, f32 g, f32 b, f32 a);
    void ClearBuffers();
};

