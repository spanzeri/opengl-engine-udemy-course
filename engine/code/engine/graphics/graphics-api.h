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

    template <typename T>
    u32 CreateBuffer(std::span<T> data);

    u32 CreateBuffer(void* data, usize buffer_size);
    void DestroyBuffer(u32 buffer);

    void BindShaderProgram(ShaderProgram* program);
    void BindMaterial(Material* material);

    void BindMesh(Mesh* mesh);
    void DrawMesh(Mesh* mesh);

    void SetClearColor(f32 r, f32 g, f32 b, f32 a);
    void ClearBuffers();
};

template <typename T>
u32 GraphicsAPI::CreateBuffer(std::span<T> data)
{
    return CreateBuffer(data.data(), data.size_bytes());
}

