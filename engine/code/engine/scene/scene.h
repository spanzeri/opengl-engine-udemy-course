#pragma once

#include "../common.h"
#include "game-object.h"

#include <vector>
#include <memory>

class Scene {
public:
    void Update(f32 dt);
    void Clear();

    GameObject* CreateObject(std::string_view name, GameObject* parent = nullptr);

    template <std::derived_from<GameObject> T>
    T* CreateObject(std::string_view name, GameObject* parent = nullptr);

    bool SetParent(GameObject* obj, GameObject* parent);

    void SetActiveCamera(GameObject* camera);
    GameObject* GetActiveCamera() const;

    std::vector<LightData> CollectLights() const;

private:
    void CollectLightsFromObject(GameObject* obj, std::vector<LightData>& out_lights) const;

private:
    std::vector<std::unique_ptr<GameObject>> m_objects;
    GameObject* m_active_camera;
};

template <std::derived_from<GameObject> T>
T* Scene::CreateObject(std::string_view name, GameObject* parent) {
    auto obj = new T();
    obj->m_scene = this;
    obj->name = name;
    SetParent(obj, parent);
    return obj;
}

