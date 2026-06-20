#pragma once

#include "../base.h"

#include "component.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <memory>
#include <string>
#include <vector>

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void Update(f32 dt) { unused(dt); }

    void    DoUpdate(f32 dt);

    bool    IsAlive() const;
    void    MarkForDestroy();

    void    AddComponent(Component* component);

    template <std::derived_from<Component> T>
    [[nodiscard]] T* GetComponent() const;

    GameObject* GetParent() const;
    bool SetParent(GameObject* parent);

    glm::vec3 GetPosition() const;
    void SetPosition(glm::vec3 pos);

    glm::vec3 GetRotation() const;
    void SetRotation(glm::vec3 rot);

    glm::vec3 GetScale() const;
    void SetScale(glm::vec3 scale);

    glm::mat4 GetTransformLocal() const;
    glm::mat4 GetTransformWorld() const;

    std::string name;
    GameObject* parent = nullptr;

protected:
    GameObject() = default;

private:
    std::vector<std::unique_ptr<GameObject>> m_children;
    std::vector<std::unique_ptr<Component>>  m_components;

    bool m_is_alive = true;

    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);

    friend class Scene;
};

template <std::derived_from<Component> T>
T* GameObject::GetComponent() const
{
    ComponentId type_id = Component::StaticTypeId<T>();
    for (auto& component : m_components) {
        if (component->GetTypeId() == type_id) {
            return (T*)(component.get());
        }
    }

    return nullptr;
}

