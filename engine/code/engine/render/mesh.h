#pragma once

#include "../base.h"
#include "../graphics/vertex-layout.h"
#include "../graphics/buffer.h"

#include <memory>

class Mesh : NonCopiable {
public:
    Mesh() = default;
    Mesh(const VertexLayout &vertex_layout, std::span<f32> vertices, std::span<u16> indices);
    Mesh(const VertexLayout &vertex_layout, std::span<f32> vertices);
    ~Mesh();

    void Bind();
    void Draw();

    static std::shared_ptr<Mesh> Load(std::string_view path);

private:
    VertexLayout m_vertex_layout = {};
    u32 m_vao = 0;
    Buffer m_vbo;
    Buffer m_ibo;

    usize m_vertex_count = 0;
    usize m_index_count = 0;
};

