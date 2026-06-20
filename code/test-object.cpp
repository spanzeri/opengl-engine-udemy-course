#include "test-object.h"

#include <engine/engine.h>
#include <engine/scene/components/mesh-component.h>

TestObject::TestObject() {
    //
    // Material
    //

    const char vertex_shader_code[] = R"(
        #version 430 core
        layout (location=0) in vec3 position;
        layout (location=1) in vec3 color;

        out vec3 v_color;
        layout (location = 0) uniform mat4 uModel;

        layout (location = 1) uniform mat4 uView;
        layout (location = 2) uniform mat4 uProjection;

        void main()
        {
            gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
            v_color = color;
        }
    )";

    const char fragment_shader_code[] = R"(
        #version 430 core

        in vec3 v_color;
        out vec4 out_frag_color;

        void main()
        {
            out_frag_color = vec4(v_color, 1.0);
        }
    )";

    auto& graphics = Engine::GetInstance().graphics;
    auto shader = graphics.CreateShaderProgram(vertex_shader_code, fragment_shader_code);
    graphics.BindShaderProgram(shader.get());

    auto material = std::make_shared<Material>(MOV(shader));

    //
    // Mesh
    //

    f32 vertices[] = {
         //    position    |     color

        // front
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        // back
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

        // left
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

        // right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

        // top
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

        // bottom
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
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
                .type   = TYPE_F32,
                .offset = 0,
            }, {
                .index  = 1,
                .size   = 3,
                .type   = TYPE_F32,
                .offset = sizeof(f32) * 3,
            }
        },
        .stride = sizeof(f32) * 6,
    };

    auto mesh = std::make_shared<Mesh>(vertex_layout, vertices, indices);

    AddComponent(new MeshComponent(MOV(material), MOV(mesh)));
}

void TestObject::Update(f32 dt) {
#if 0
    const auto &input = Engine::GetInstance().input;

    if (input.IsKeyDown(Key::A)) {
        SetPosition(GetPosition() - glm::vec3(1, 0, 0) * dt);
    }
    if (input.IsKeyDown(Key::D)) {
        SetPosition(GetPosition() + glm::vec3(1, 0, 0) * dt);
    }
    if (input.IsKeyDown(Key::W)) {
        SetPosition(GetPosition() + glm::vec3(0, 1, 0) * dt);
    }
    if (input.IsKeyDown(Key::S)) {
        SetPosition(GetPosition() - glm::vec3(0, 1, 0) * dt);
    }

    if (input.IsKeyDown(Key::J)) {
        SetRotation(GetRotation() - glm::vec3(0, 1, 0) * dt);
    }
    if (input.IsKeyDown(Key::L)) {
        SetRotation(GetRotation() + glm::vec3(0, 1, 0) * dt);
    }
    if (input.IsKeyDown(Key::K)) {
        SetRotation(GetRotation() - glm::vec3(1, 0, 0) * dt);
    }
    if (input.IsKeyDown(Key::I)) {
        SetRotation(GetRotation() + glm::vec3(1, 0, 0) * dt);
    }
#endif
}
