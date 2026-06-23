#pragma once

#include "application.h"
#include "graphics/graphics-api.h"
#include "input-manager.h"
#include "io/filesystem.h"
#include "render/render-queue.h"
#include "scene/scene.h"

#include <memory>
#include <chrono>

using hrclock = std::chrono::high_resolution_clock;
struct GLFWwindow;

class Engine
{
private:
    Engine() = default;

public:
    static Engine &GetInstance();

    bool Init(s32 width, s32 height);
    void Shutdown();
    void Run();

    void SetApplication(Application* app);
    Application* SetApplication();

    void SetCurrentScene(Scene* scene);
    Scene* GetCurrentScene() const;

public:
    InputManager input;
    GraphicsAPI  graphics;
    RenderQueue  render_queue;
    FileSystem   file_system;

private:
    std::unique_ptr<Application> m_application;
    std::unique_ptr<Scene> m_current_scene;

    hrclock::time_point m_last_time;
    GLFWwindow *m_window;
};

