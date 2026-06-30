#include "mesh.h"

#include "../engine.h"

#include <glad/gl.h>

#define CGLTF_IMPLEMENTATION 1
#include <cgltf.h>

Mesh::Mesh(const VertexLayout &vertex_layout, std::span<f32> vertices, std::span<u16> indices)
    : m_vertex_layout(vertex_layout), m_index_count(indices.size())
{
    m_vbo = Buffer(Buffer::Type::Vertex, vertices, false);
    m_ibo = Buffer(Buffer::Type::Index, indices, false);

    glCreateVertexArrays(1, &m_vao);

    for (u32 ai = 0; ai < vertex_layout.elements.size(); ai++) {
        auto& vl = vertex_layout.elements[ai];
        glEnableVertexArrayAttrib(m_vao, vl.index);
        glVertexArrayAttribFormat(m_vao, vl.index, vl.size, (u32)vl.type, GL_FALSE, vl.offset);
        glVertexArrayAttribBinding(m_vao, vl.index, 0);
    }
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo.GetId(), 0, vertex_layout.stride);
    glVertexArrayElementBuffer(m_vao, m_ibo.GetId());

    ASSERT(
        (vertices.size_bytes() % m_vertex_layout.stride) == 0,
        "The size of the vertices isn't a multiple of the size of the vertex layout. Something is out of sync");
    m_vertex_count = vertices.size_bytes() / m_vertex_layout.stride;
}

Mesh::Mesh(const VertexLayout &vertex_layout, std::span<f32> vertices)
    : m_vertex_layout(vertex_layout), m_vertex_count(vertices.size()), m_index_count(0)
{
    m_vbo = Buffer(Buffer::Type::Vertex, vertices, false);

    glCreateVertexArrays(1, &m_vao);

    for (u32 ai = 0; ai < vertex_layout.elements.size(); ai++) {
        auto& vl = vertex_layout.elements[ai];
        glEnableVertexArrayAttrib(m_vao, vl.index);
        glVertexArrayAttribFormat(m_vao, vl.index, vl.size, (u32)vl.type, GL_FALSE, vl.offset);
        glVertexArrayAttribBinding(m_vao, vl.index, 0);
    }
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo.GetId(), 0, vertex_layout.stride);

    ASSERT(
        (vertices.size_bytes() % m_vertex_layout.stride) == 0,
        "The size of the vertices isn't a multiple of the size of the vertex layout. Something is out of sync");
    m_vertex_count = vertices.size_bytes() / m_vertex_layout.stride;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    m_vao = 0;
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

std::shared_ptr<Mesh> Mesh::Load(std::string_view path) {
    auto& fs = Engine::GetInstance().file_system;

    auto contents = fs.LoadAssetTextFile(path);
    if (contents.empty()) {
        return nullptr;
    }

    auto ReadFloats = [](const cgltf_accessor* acc, cgltf_size i, f32* out, int n) {
        std::fill(out, out + n, 0.0f);
        return cgltf_accessor_read_float(acc, i, out, n);
    };

    auto ReadIndex = [](const cgltf_accessor* acc, cgltf_size i) {
        u32 out = 0;
        cgltf_bool ok = cgltf_accessor_read_uint(acc, i, &out, 1);
        ASSERT(out < 0xFFFF);
        return ok ? (u16)out : 0;
    };

    cgltf_options options = {};
    cgltf_data* data = nullptr;

    cgltf_result res = cgltf_parse(&options, contents.data(), contents.size(), &data);
    if (res != cgltf_result_success) {
        ERROR("cgltf parse failed with error: {}", (int)res);
        return nullptr;
    }
    defer { cgltf_free(data); };

    auto fullpath = fs.GetAssetsDir() / path;
    res = cgltf_load_buffers(&options, data, fullpath.remove_filename().string().c_str());
    if (res != cgltf_result_success) {
        ERROR("cgltf buffer load failed with error: {}", (int)res);
        return nullptr;
    }

    std::shared_ptr<Mesh> result = nullptr;

    for (cgltf_size mi = 0; mi < data->meshes_count; ++mi) {
        auto& mesh = data->meshes[mi];
        for (cgltf_size pi = 0; pi < mesh.primitives_count; ++pi) {
            auto& primitive = mesh.primitives[pi];
            if (primitive.type != cgltf_primitive_type_triangles) {
                // @TODO: Report an error
                continue;
            }

            VertexLayout vertex_layout = {};
            cgltf_accessor* accessors[4] = {};

            for (cgltf_size ai = 0; ai < primitive.attributes_count; ai++) {
                auto& attribute = primitive.attributes[ai];
                auto accessor = attribute.data;
                if (!accessor) {
                    continue;
                }

                VertexElement element;
                element.type = VertexElement::Type::F32;

                switch (attribute.type) {
                    case cgltf_attribute_type_position: {
                        accessors[VertexElement::PositionIndex] = accessor;
                        element.index = VertexElement::PositionIndex;
                        element.size = 3;
                    } break;

                    case cgltf_attribute_type_normal: {
                        accessors[VertexElement::NormalIndex] = accessor;
                        element.index = VertexElement::NormalIndex;
                        element.size = 3;
                    } break;

                    case cgltf_attribute_type_color: {
                        if (attribute.index != 0) {
                            // Only get the first color attribute
                            break;
                        }

                        accessors[VertexElement::ColorIndex] = accessor;
                        element.index = VertexElement::ColorIndex;
                        element.size = 3;
                    } break;

                    case cgltf_attribute_type_texcoord: {
                        if (attribute.index != 0) {
                            // Only get the first uv coordinates attribute
                            break;
                        }

                        accessors[VertexElement::UVIndex] = accessor;
                        element.index = VertexElement::UVIndex;
                        element.size = 2;
                    } break;

                    default:
                        break;
                }

                if (element.size > 0) {
                    element.offset = vertex_layout.stride;
                    vertex_layout.stride += element.size * sizeof(float);
                    vertex_layout.elements.push_back(element);
                }
            }

            if (!accessors[VertexElement::PositionIndex]) {
                // @TODO: Report an error?
                continue;
            }

            auto vertex_count = accessors[VertexElement::PositionIndex]->count;

            std::vector<f32> vertices;
            vertices.resize(vertex_layout.stride / sizeof(f32) * vertex_count);

            for (cgltf_size vi = 0; vi < vertex_count; vi++) {
                for (auto& element : vertex_layout.elements) {
                    if (!accessors[element.index]) {
                        continue;
                    }

                    auto index = (vi * vertex_layout.stride + element.offset) / sizeof(f32);
                    f32* out_data = &vertices[index];
                    ReadFloats(accessors[element.index], vi, out_data, element.size);
                }
            }

            if (primitive.indices) {
                auto index_count = primitive.indices->count;
                std::vector<u16> indices(index_count);
                for (cgltf_size i = 0; i < index_count; i++) {
                    indices[i] = ReadIndex(primitive.indices, i);
                }
                result = std::make_shared<Mesh>(vertex_layout, vertices, indices);
            }
            else {
                result = std::make_shared<Mesh>(vertex_layout, vertices);
            }

            if (result) {
                break;
            }
        }

        if (result) {
            break;
        }
    }

    return result;
}

std::shared_ptr<Mesh> Mesh::CreateCube() {
    f32 vertices[] = {
         //    position    |     color      | nornal |    uvs
        // front
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0, 0, 1, 0, 0,
         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1, 1, 0,
         0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0, 0, 1, 1, 1,
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1, 0, 1,

        // back
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, 0, -1, 0, 0,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, -1, 1, 0,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0, 0, -1, 1, 1,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0, 0, -1, 0, 1,

        // left
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1, 0, 0, 0, 0,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1, 0, 0, 0, 1,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, -1, 0, 0, 1, 1,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, -1, 0, 0, 1, 0,

        // right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1, 0, 0, 0, 0,
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1, 0, 0, 0, 1,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1, 0, 0, 1, 1,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1, 0, 0, 1, 0,

        // bottom
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, -1, 0, 0, 0,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0, -1, 0, 1, 0,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0, -1, 0, 1, 1,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0, -1, 0, 0, 1,

        // top
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, 1, 0, 0, 0,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 1, 0, 1, 0,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0, 1, 0, 1, 1,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0, 1, 0, 0, 1,
    };

    u16 indices[] = {
        // front
        0, 1, 2,
        0, 2, 3,
        //back
        6, 5, 4,
        7, 6, 4,
        // left
        10, 9, 8,
        11, 10, 8,
        // right
        12, 13, 14,
        12, 14, 15,
        // top
        16, 17, 18,
        16, 18, 19,
        // bottom
        22, 21, 20,
        23, 22, 20,
    };

    auto vertex_layout = VertexLayout{
        .elements = { {
                .index  = 0,
                .size   = 3,
                .type   = VertexElement::Type::F32,
                .offset = 0,
            }, {
                .index  = 1,
                .size   = 3,
                .type   = VertexElement::Type::F32,
                .offset = sizeof(f32) * 3,
            }, {
                .index  = 2,
                .size   = 3,
                .type   = VertexElement::Type::F32,
                .offset = sizeof(f32) * 6,
            }, {
                .index  = 3,
                .size   = 2,
                .type   = VertexElement::Type::F32,
                .offset = sizeof(f32) * 9,
            }
        },
        .stride = sizeof(f32) * 11, // 3 pos, 3 nml, 3 color and 2 uv
    };

    return std::make_shared<Mesh>(vertex_layout, vertices, indices);
}

