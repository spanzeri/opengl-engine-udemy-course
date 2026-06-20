#pragma once

#include <engine/application.h>
#include <engine/scene/scene.h>

class Game : public Application
{
public:
    bool Init();
    void Shutdown();
    void Update(f32 dt_sec);

private:
};

