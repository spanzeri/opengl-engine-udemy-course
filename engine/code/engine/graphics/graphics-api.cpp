#include "graphics-api.h"

#include <glad/gl.h>

static GLuint CreateShader(GLenum type, std::string_view source);
static GLuint LinkProgram(std::span<GLuint> shaders);

std::shared_ptr<ShaderProgram> GraphicsAPI::s_default_shader_program;

void GraphicsAPI::Init() {

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);
}

void GraphicsAPI::Shutdown() {
    s_default_shader_program.reset();
}

std::shared_ptr<ShaderProgram> GraphicsAPI::CreateShaderProgram(std::string_view vertex_source, std::string_view fragment_source) const
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

    GLuint shaders[] = { vertex_shader, fragment_shader };
    GLuint program = LinkProgram(shaders);
    if (program == 0 ) {
        return nullptr;
    }

    return std::make_shared<ShaderProgram>(program);
}

std::shared_ptr<ShaderProgram> GraphicsAPI::GetDefaultShaderProgram() const {
    auto MakeDefaultProgram = [this]() -> bool {
        auto default_vertex_shader_code = R"(
            #version 430 core

            layout (location=0) in vec3 position;
            layout (location=1) in vec3 color;
            layout (location=2) in vec3 normal;
            layout (location=3) in vec2 uv;

            out vec3 vColor;
            out vec2 vUV;
            out vec3 vFragPosition;
            out vec3 vNormal;

            layout(std140, binding = 0) uniform FrameData {
                mat4  view;
                mat4  projection;
                vec4  cameraPos;
                float time;
            };

            layout (location = 0) uniform mat4 uModel;

            void main()
            {
                vFragPosition = (uModel * vec4(position, 1.0)).xyz;
                vNormal = mat3(transpose(inverse(uModel))) * normal;
                gl_Position = projection * view * vec4(vFragPosition, 1);
                vColor = color;
                vUV = uv;
            }
        )";

        auto default_fragment_shader_code = R"(
            #version 430 core

            in vec3 vColor;
            in vec2 vUV;
            in vec3 vFragPosition;
            in vec3 vNormal;

            out vec4 out_frag_color;

            struct Light {
                vec3 color;
                vec3 position;
            };
            uniform Light uLight;

            layout(std140, binding = 0) uniform FrameData {
                mat4  view;
                mat4  projection;
                vec4  cameraPos;
                float time;
            };

            uniform float roughness;
            uniform vec2 uvOffset;

            layout(binding = 0) uniform sampler2D albedo;

            void main()
            {
                // Diffuse
                vec3 light_dir = normalize(uLight.position - vFragPosition);
                float diffuse_amount = max(0, dot(light_dir, vNormal));
                vec3 diffuse = diffuse_amount * uLight.color;

                // Specular
                vec3 view_dir = normalize(cameraPos.xyz - vFragPosition);
                vec3 reflected_dir = reflect(-light_dir, vNormal);
                float specular_amount = pow(max(0, dot(view_dir, reflected_dir)), 32.0);
                float specular_strength = 0.5;
                vec3 specular = specular_strength * specular_amount * uLight.color;

                // Ambient
                vec3 ambient = 0.02 * uLight.color;

                vec3 tex_color = texture(albedo, vUV + uvOffset).rgb;
                out_frag_color = vec4(tex_color * (ambient + diffuse + specular), 1.0);
            }
        )";

        s_default_shader_program = CreateShaderProgram(default_vertex_shader_code, default_fragment_shader_code);
        return s_default_shader_program != nullptr;
    };

    static bool default_shader_created_succesfully = MakeDefaultProgram();
    ASSERT(default_shader_created_succesfully, "Failed to create default shader program");

    return s_default_shader_program;
}

void GraphicsAPI::SetClearColor(f32 r, f32 g, f32 b, f32 a) const {
    glClearColor(r, g, b, a);
}

void GraphicsAPI::ClearBuffers() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static GLuint CreateShader(GLenum type, std::string_view source) {
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

static GLuint LinkProgram(std::span<GLuint> shaders) {
    GLuint program = glCreateProgram();

    for (auto shader : shaders) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0) {
        char compile_error[512] = {};
        glGetProgramInfoLog(program, sizeof(compile_error), nullptr, compile_error);
        ERROR("Shader linking error: {}", compile_error);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

