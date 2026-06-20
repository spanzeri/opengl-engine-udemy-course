#pragma once

#include "../component.h"

#include "../../render/mesh.h"
#include "../../render/material.h"

#include <memory>

class MeshComponent : public Component {
    COMPONENT(MeshComponent);

public:
    MeshComponent(std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh);
    void Update(f32 dt) override;

private:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
};

