#include "mesh.h"

#include "../engine.h"
#include "../graphics/graphics-api.h"

#include <glad/gl.h>

Mesh::Mesh(const VertexLayout &vertex_layout, std::span<f32> vertices, std::span<u16> indices)
    : m_vertex_layout(vertex_layout), m_index_count(indices.size())
{
    auto &graphics = Engine::GetInstance().graphics;
    m_vbo = graphics.CreateBuffer(vertices);
    m_ibo = graphics.CreateBuffer(indices);

    glCreateVertexArrays(1, &m_vao);

    for (u32 ai = 0; ai < vertex_layout.elements.size(); ai++) {
        auto& vl = vertex_layout.elements[ai];
        glEnableVertexArrayAttrib(m_vao, vl.index);
        glVertexArrayAttribFormat(m_vao, vl.index, vl.size, vl.type, GL_FALSE, vl.offset);
        glVertexArrayAttribBinding(m_vao, vl.index, 0);
    }
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, vertex_layout.stride);
    glVertexArrayElementBuffer(m_vao, m_ibo);

    ASSERT(
        (vertices.size_bytes() % m_vertex_layout.stride) == 0,
        "The size of the vertices isn't a multiple of the size of the vertex layout. Something is out of sync");
    m_vertex_count = vertices.size_bytes() / m_vertex_layout.stride;
}

Mesh::Mesh(const VertexLayout &vertex_layout, std::span<f32> vertices)
    : m_vertex_layout(vertex_layout), m_vertex_count(vertices.size()), m_index_count(0)
{
    auto &graphics = Engine::GetInstance().graphics;
    m_vbo = graphics.CreateBuffer(vertices);

    glCreateVertexArrays(1, &m_vao);

    for (u32 ai = 0; ai < vertex_layout.elements.size(); ai++) {
        auto& vl = vertex_layout.elements[ai];
        glEnableVertexArrayAttrib(m_vao, vl.index);
        glVertexArrayAttribFormat(m_vao, vl.index, vl.size, vl.type, GL_FALSE, vl.offset);
        glVertexArrayAttribBinding(m_vao, vl.index, 0);
    }
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, vertex_layout.stride);

    ASSERT(
        (vertices.size_bytes() % m_vertex_layout.stride) == 0,
        "The size of the vertices isn't a multiple of the size of the vertex layout. Something is out of sync");
    m_vertex_count = vertices.size_bytes() / m_vertex_layout.stride;
}

Mesh::~Mesh()
{
    auto &graphics = Engine::GetInstance().graphics;
    graphics.DestroyBuffer(m_ibo);
    graphics.DestroyBuffer(m_vbo);
    glDeleteVertexArrays(1, &m_vao);

    m_ibo = m_vbo = m_vao = 0;
}

void Mesh::Bind()
{
    glBindVertexArray(m_vao);
}

void Mesh::Draw()
{
    if (m_index_count > 0) {
        glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_SHORT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
    }
}


