#pragma once

#include <engine/scene/game-object.h>

#include <engine/render/material.h>
#include <engine/render/mesh.h>

class TestObject : public GameObject {
public:
    TestObject();

    void Update(f32 dt) override;

private:
};

