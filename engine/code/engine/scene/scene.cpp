#include "scene.h"

#include "game-object.h"
#include "components/light-component.h"

#include <algorithm>

void Scene::Update(f32 dt) {
    for (auto it = m_objects.begin(); it != m_objects.end();) {
        if ((*it)->IsAlive()) {
            (*it)->DoUpdate(dt);
            it++;
        } else {
            m_objects.erase(it);
        }
    }
}

void Scene::Clear() {
    m_objects.clear();
}

GameObject* Scene::CreateObject(std::string_view name, GameObject* parent) {
    return CreateObject<GameObject>(name, parent);
}

bool Scene::SetParent(GameObject* obj, GameObject* parent) {
    bool result = false;
    auto curr_parent = obj->GetParent();

    if (parent == nullptr) {
        if (curr_parent != nullptr) {
            auto it = std::find_if(curr_parent->m_children.begin(), curr_parent->m_children.end(), [obj](const auto& child) {
                return child.get() == obj;
            });
            ASSERT(it != curr_parent->m_children.end());

            if (it != curr_parent->m_children.end()) {
                m_objects.push_back(MOV(*it));
                obj->m_parent = nullptr;
                curr_parent->m_children.erase(it);
                result = true;
            }
        } else {
            // Currently it has no parent
            //  1. It is a scene root
            //  2. It is a brand new object
            auto it = std::find_if(m_objects.begin(), m_objects.end(), [obj](const auto& child) {
                return child.get() == obj;
            });
            if (it == m_objects.end()) {
                // This is new
                m_objects.push_back(std::unique_ptr<GameObject>(obj));
                obj->m_parent = nullptr;
                result = true;
            }
        }
    } else {
        // We are trying to make it into a child of an object
        if (curr_parent != nullptr) {
            auto it = std::find_if(curr_parent->m_children.begin(), curr_parent->m_children.end(), [obj](const auto& child) {
                return child.get() == obj;
            });

            ASSERT(it != curr_parent->m_children.end());
            if (it != curr_parent->m_children.end()) {
                bool found = false;
                auto current_element = parent;
                while (current_element) {
                    if (current_element == obj) {
                        found = true;
                        break;
                    }
                    current_element = current_element->m_parent;
                }

                if (!found) {
                    parent->m_children.push_back(MOV(*it));
                    obj->m_parent = parent;
                    curr_parent->m_children.erase(it);
                    result = true;
                }
            }
        } else {
            // This was a root level object or a new one
            auto it = std::find_if(m_objects.begin(), m_objects.end(), [obj](const auto& child) {
                return child.get() == obj;
            });

            if (it == m_objects.end()) {
                parent->m_children.push_back(std::unique_ptr<GameObject>(obj));
                obj->m_parent = parent;
                result = true;
            } else {
                bool found = false;
                auto current_element = parent;
                while (current_element) {
                    if (current_element == obj) {
                        found = true;
                        break;
                    }
                    current_element = current_element->m_parent;
                }

                if (!found) {
                    parent->m_children.push_back(MOV(*it));
                    obj->m_parent = parent;
                    m_objects.erase(it);
                    result = true;
                }
            }
        }
    }

    return result;
}

void Scene::SetActiveCamera(GameObject* camera) {
    m_active_camera = camera;
}

 GameObject* Scene::GetActiveCamera() const {
    return m_active_camera;
}

std::vector<LightData> Scene::CollectLights() const {
    auto result = std::vector<LightData>();
    for (const auto& obj : m_objects) {
        CollectLightsFromObject(obj.get(), result);
    }
    return result;
}

void Scene::CollectLightsFromObject(GameObject* obj, std::vector<LightData>& out_lights) const {
    if (!obj) {
        return;
    }

    if (auto* light = obj->GetComponent<LightComponent>()) {
        out_lights.push_back({ .color = light->color, .position = obj->GetWorldPosition() });
    }

    for (auto& child : obj->m_children) {
        CollectLightsFromObject(child.get(), out_lights);
    }
}

