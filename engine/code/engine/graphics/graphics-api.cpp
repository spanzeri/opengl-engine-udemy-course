#include "graphics-api.h"

#include "shader-program.h"
#include "../render/material.h"
#include "../render/mesh.h"

#include <glad/gl.h>

static GLuint CreateShader(GLenum type, std::string_view source);
std::shared_ptr<ShaderProgram> GraphicsAPI::CreateShaderProgram(std::string_view vertex_source, std::string_view fragment_source)
{
    GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_source);
    if (vertex_shader == 0) {
        return nullptr;
    }
    defer { glDeleteShader(vertex_shader); };

    GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_source);
    if (fragment_shader == 0) {
        return nullptr;
    }
    defer { glDeleteShader(fragment_shader); };

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0) {
        char compile_error[512] = {};
        glGetProgramInfoLog(program, sizeof(compile_error), nullptr, compile_error);
        ERROR("Shader linking error: {}", compile_error);
        glDeleteProgram(program);
        return nullptr;
    }

    return std::make_shared<ShaderProgram>(program);
}

u32 GraphicsAPI::CreateBuffer(void* data, usize buffer_size)
{
    u32 bo;
    glCreateBuffers(1, &bo);
    glNamedBufferStorage(bo, buffer_size, data, 0);
    return bo;
}

void GraphicsAPI::DestroyBuffer(u32 buffer)
{
    glDeleteBuffers(1, &buffer);
}

void GraphicsAPI::BindShaderProgram(ShaderProgram* program) {
    if (program) {
        program->Bind();
    }
}

void GraphicsAPI::BindMaterial(Material *material) {
    material->Bind();
}

void GraphicsAPI::BindMesh(Mesh* mesh) {
    if (mesh) {
        mesh->Bind();
    }
}

void GraphicsAPI::DrawMesh(Mesh* mesh) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    if (mesh) {
        mesh->Draw();
    }
}

void GraphicsAPI::SetClearColor(f32 r, f32 g, f32 b, f32 a) {
    glClearColor(r, g, b, a);
}

void GraphicsAPI::ClearBuffers() {
    glClear(GL_COLOR_BUFFER_BIT);
}

static GLuint CreateShader(GLenum type, std::string_view source)
{
    GLuint shader = glCreateShader(type);

    const char *sources[] = { source.data() };
    s32 lengths[] = { (s32)source.size() };

    glShaderSource(shader, 1, sources, lengths);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == 0) {
        char compile_error[512] = {};
        glGetShaderInfoLog(shader, sizeof(compile_error), nullptr, compile_error);
        ERROR("Shader compilation error: {}", compile_error);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

