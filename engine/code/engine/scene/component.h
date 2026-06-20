#pragma once

#include "../base.h"

class GameObject;

enum struct ComponentId : u64 {};

class Component {
public:
    virtual ~Component() = default;

    virtual ComponentId GetTypeId() const = 0;

    virtual void Update(f32 dt) = 0;

    GameObject* GetOwner() const;

    template <typename T>
    static ComponentId StaticTypeId()
    {
        static ComponentId id = (ComponentId)(++next_component_id);
        return id;
    }

protected:
    GameObject* m_owner = nullptr;

    friend class GameObject;

private:
    static u64 next_component_id;
};

#define COMPONENT(ComponentClass) \
    public: \
        ComponentId GetTypeId() const override { return ComponentClass::StaticTypeId<ComponentClass>(); } \
        inline static ComponentId TypeId = ComponentClass::StaticTypeId<ComponentClass>()

