#include "game.h"

#include <engine/engine.h>
#include <engine/graphics/shader-program.h>
#include <engine/graphics/texture.h>
#include <engine/scene/components/camera-component.h>
#include <engine/scene/components/mesh-component.h>
#include <engine/scene/components/player-controller-component.h>

bool Game::Init()
{
    //
    // Material
    //

    auto material = Material::Load("material/brick.mat");

    // auto vertex_shader_text = fs.LoadAssetTextFile("shader/default.vert.glsl");
    // auto fragment_shader_text = fs.LoadAssetTextFile("shader/default.frag.glsl");
    //
    // auto& graphics = Engine::GetInstance().graphics;
    // auto shader = graphics.CreateShaderProgram(vertex_shader_text, fragment_shader_text);
    // graphics.BindShaderProgram(shader.get());
    //
    // auto material = std::make_shared<Material>(MOV(shader));
    // // Load texture
    // material->SetParam(0, Texture::Load("texture/brick.png"));

    //
    // Mesh
    //

    f32 vertices[] = {
         //    position    |     color      |    uvs
        // front
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0, 0,
         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1, 0,
         0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1, 1,
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 1,

        // back
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, 0,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1, 0,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1, 1,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0, 1,

        // left
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, 0,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 1,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1, 1,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1, 0,

        // right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, 0,
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 1,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1, 1,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1, 0,

        // top
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, 0,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1, 0,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1, 1,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0, 1,

        // bottom
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0, 0,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1, 0,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1, 1,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0, 1,
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
            }, {
                .index  = 2,
                .size   = 2,
                .type   = TYPE_F32,
                .offset = sizeof(f32) * 6,
            }
        },
        .stride = sizeof(f32) * 8, // 3 pos, 3 nml and 2 uv
    };

    auto mesh = std::make_shared<Mesh>(vertex_layout, vertices, indices);

    auto* scene = new Scene();
    Engine::GetInstance().SetCurrentScene(scene);

    auto cube1 = scene->CreateObject<GameObject>("Cube1");
    cube1->AddComponent(new MeshComponent(material, mesh));
    cube1->SetPosition(glm::vec3(-1, 0, 0));

    auto cube2 = scene->CreateObject<GameObject>("Cube2");
    cube2->AddComponent(new MeshComponent(material, mesh));
    cube2->SetPosition(glm::vec3( 1, 0, 0));

    auto* player = scene->CreateObject("Player");
    player->AddComponent(new PlayerControllerComponent());

    auto* camera = scene->CreateObject("Camera", player);
    camera->AddComponent(new CameraComponent());
    camera->SetPosition(glm::vec3(0, 0, 1));
    scene->SetActiveCamera(camera);

    return true;
}

void Game::Shutdown()
{
}

void Game::Update(f32 dt_sec)
{
    const auto &input = Engine::GetInstance().input;
    if (input.IsKeyJustPressed(Key::Escape)) {
        SetNeedsToBeClosed();
    }
}
