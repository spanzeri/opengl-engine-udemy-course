#include "game.h"

#include <engine/engine.h>
#include <engine/graphics/shader-program.h>
#include <engine/graphics/texture.h>
#include <engine/scene/components/animation-component.h>
#include <engine/scene/components/camera-component.h>
#include <engine/scene/components/light-component.h>
#include <engine/scene/components/mesh-component.h>
#include <engine/scene/components/player-controller-component.h>

bool Game::Init() {
    //
    // Material
    //

    auto material = Material::Load("material/brick.mat");

    //
    // Mesh
    //

    auto cube_mesh = Mesh::CreateCube();

    auto* scene = new Scene();
    Engine::GetInstance().SetCurrentScene(scene);

    auto cube1 = scene->CreateObject<GameObject>("Cube1");
    cube1->AddComponent(new MeshComponent(material, cube_mesh));
    cube1->SetPosition(glm::vec3(-1, 0, 0));

    auto cube2 = scene->CreateObject<GameObject>("Cube2");
    cube2->AddComponent(new MeshComponent(material, cube_mesh));
    cube2->SetPosition(glm::vec3( 1, 0, 0));

    auto suzanne_obj = GameObject::LoadGLTF("model/suzanne/Suzanne.gltf");
    suzanne_obj->SetPosition(glm::vec3(-3, 1, 0));

    auto* player = scene->CreateObject("Player");
    player->AddComponent(new PlayerControllerComponent());

    auto* camera = scene->CreateObject("Camera", player);
    camera->AddComponent(new CameraComponent());
    camera->SetPosition(glm::vec3(0, 0, 1));
    scene->SetActiveCamera(camera);

    // auto gun_obj = GameObject::LoadGLTF("model/service_pistol/service_pistol.gltf");
    // gun_obj->SetParent(camera);
    // gun_obj->SetPosition(glm::vec3(0.15f, -0.13f, -0.3f));
    auto gun_obj = GameObject::LoadGLTF("model/sten_gunmachine_carbine/scene.gltf");
    gun_obj->SetParent(camera);
    gun_obj->SetPosition(glm::vec3(0.75f, -0.5f, -0.75f));
    gun_obj->SetScale(glm::vec3(-1, 1, 1));
    if (auto* anim_component = gun_obj->GetComponent<AnimationComponent>()) {
        if (auto bullet = gun_obj->FindChildByName("bullet_33")) {
            bullet->SetActive(false);
        }

        if (auto fire = gun_obj->FindChildByName("BOOM_35")) {
            fire->SetActive(false);
        }

        anim_component->Play("shoot", true);
    }

    auto* point_light = scene->CreateObject("PointLight");
    point_light->AddComponent(new LightComponent(glm::vec3(1.0f, 0.8f, 0.8f)));
    point_light->SetPosition(glm::vec3(0.f, 2.5f, 1.5f));

    return true;
}

void Game::Shutdown() {
}

void Game::Update(f32 dt_sec) {
    const auto &input = Engine::GetInstance().input;
    if (input.IsKeyJustPressed(Key::Escape)) {
        SetNeedsToBeClosed();
    }
}

