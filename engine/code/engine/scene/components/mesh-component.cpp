#include "mesh-component.h"

#include "../../engine.h"
#include "../game-object.h"

MeshComponent::MeshComponent(std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh)
    : m_material(material), m_mesh(mesh) {
}

void MeshComponent::Update(f32 dt) {
    if (!m_material || !m_mesh) {
        return;
    }

    Engine::GetInstance().render_queue.Submit({
        .mesh      = m_mesh.get(),
        .material  = m_material.get(),
        .model_mat = m_owner->GetTransformWorld(),
    });
}

