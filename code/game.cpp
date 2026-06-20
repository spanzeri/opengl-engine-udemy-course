#include "game.h"

#include <engine/engine.h>
#include <engine/graphics/shader-program.h>
#include <engine/scene/components/camera-component.h>
#include <engine/scene/components/player-controller-component.h>

#include "test-object.h"

bool Game::Init()
{
    auto* scene = new Scene();
    Engine::GetInstance().SetCurrentScene(scene);

    scene->CreateObject<TestObject>("Test object");

    auto* player = scene->CreateObject("Player");
    player->AddComponent(new PlayerControllerComponent());

    auto* camera = scene->CreateObject("Camera", player);
    camera->AddComponent(new CameraComponent());
    camera->SetPosition(glm::vec3(0, 0, 2));
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
