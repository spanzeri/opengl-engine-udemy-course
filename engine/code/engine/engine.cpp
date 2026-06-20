#include "engine.h"

#include "application.h"
#include "scene/components/camera-component.h"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

static Key key_from_glfw(s32 glfw_key) {
    // Those need to match
    return (Key)glfw_key;
}

static Button buttom_from_glfw(s32 button) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_RIGHT:   return Button::Right;
        case GLFW_MOUSE_BUTTON_LEFT:    return Button::Left;
        case GLFW_MOUSE_BUTTON_MIDDLE:  return Button::Middle;
        case GLFW_MOUSE_BUTTON_4:       return Button::Thumb1;
        case GLFW_MOUSE_BUTTON_5:       return Button::Thumb2;
        case GLFW_MOUSE_BUTTON_6:       return Button::Thumb3;
        case GLFW_MOUSE_BUTTON_7:       return Button::Thumb4;
        case GLFW_MOUSE_BUTTON_8:       return Button::Thumb5;
    }
    return Button::COUNT;
}

Engine &Engine::GetInstance()
{
    static Engine instance;
    return instance;
}

bool Engine::Init(s32 width, s32 height)
{
    if (!m_application) {
        ERROR("Missing application while initializing the engine");
        return false;
    }

#if CONFIG_DEBUG
    // This is here because renderdoc doesn't work right with wayland.
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    if (!glfwInit()) {
        ERROR("Failed to initialize GLFW");
        return false;
    }

    glfwSetErrorCallback([](s32 error_code, const char *description) {
        ERROR("GLFW Error[{}]: {}", error_code, description);
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, "OpenGL Engine", nullptr, nullptr);
    if (!m_window) {
        ERROR("Failed to create window");
        return false;
    }
    glfwMakeContextCurrent(m_window);

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        Engine::GetInstance().input.SetKeyPressed(key_from_glfw(key), action != GLFW_RELEASE);
    });
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        Engine::GetInstance().input.SetButtonPressed(buttom_from_glfw(button), action != GLFW_RELEASE);
    });
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
        Engine::GetInstance().input.SetMousePosition((f32)x, (f32)y);
    });
    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Engine::GetInstance().input.SetMouseScroll((f32)yoffset);
    });

    if (s32 version = gladLoadGL(glfwGetProcAddress); version == 0) {
        ERROR("Failed to initialize OpenGL");
        return false;
    }
    else {
        INFO("Loaded OpenGL version: {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    }

#if CONFIG_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              GLchar const* message,
                              void const* user_param)
        {
            const char *src_str = "OTHER";
            switch (source) {
                case GL_DEBUG_SOURCE_API:             src_str = "API";              break;
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   src_str = "WINDOW SYSTEM";    break;
                case GL_DEBUG_SOURCE_SHADER_COMPILER: src_str = "SHADER COMPILER";  break;
                case GL_DEBUG_SOURCE_THIRD_PARTY:     src_str = "THIRD PARTY";      break;
                case GL_DEBUG_SOURCE_APPLICATION:     src_str = "APPLICATION";      break;
            }
            const char *type_str = "OTHER";
            switch (type) {
                case GL_DEBUG_TYPE_ERROR:               type_str = "ERROR";                 break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "DEPRECATED_BEHAVIOR";   break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_str = "UNDEFINED_BEHAVIOR";    break;
                case GL_DEBUG_TYPE_PORTABILITY:         type_str = "PORTABILITY";           break;
                case GL_DEBUG_TYPE_PERFORMANCE:         type_str = "PERFORMANCE";           break;
                case GL_DEBUG_TYPE_MARKER:              type_str = "MARKER";                break;
            }
            if (severity == GL_DEBUG_SEVERITY_NOTIFICATION || severity == GL_DEBUG_SEVERITY_LOW) {
                INFO("GL: {}, {}, {}: {}", src_str, type_str, id, message);
            }
            else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
                WARNING("GL: {}, {}, {}: {}", src_str, type_str, id, message);
            }
            else {
                ERROR("GL: {}, {}, {}: {}", src_str, type_str, id, message);
                DEBUGBREAK();
            }
        }, nullptr);
#endif

    return m_application->Init();
}

void Engine::Shutdown()
{
    if (m_current_scene) {
        m_current_scene->Clear();
        m_current_scene.reset();
    }

    if (m_application) {
        m_application->Shutdown();
        m_application.reset();
    }

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
}

void Engine::Run()
{
    if (!m_application) {
        return;
    }

    m_last_time = hrclock::now();
    while (m_application && !m_application->NeedsToBeClosed() && !glfwWindowShouldClose(m_window)) {
        input.StartFrame();
        glfwPollEvents();

        auto now = hrclock::now();
        float dtsec = std::chrono::duration<float>(now - m_last_time).count();
        m_last_time = now;

        m_application->Update(dtsec);
        if (m_current_scene) {
            m_current_scene->Update(dtsec);
        }

        graphics.SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        graphics.ClearBuffers();

        int win_w, win_h;
        glfwGetFramebufferSize(m_window, &win_w, &win_h);
        f32 aspect = (f32)win_w / (f32)win_h;

        CameraData camera_data;
        if (m_current_scene) {
            if (auto camera_object  = m_current_scene->GetActiveCamera()) {
                auto* camera_comp = camera_object->GetComponent<CameraComponent>();
                ASSERT(camera_comp, "The camera object doesn't have a camera component associated");

                if (camera_comp) {
                    camera_data.view_matrix = camera_comp->GetViewMatrix();
                    camera_data.proj_matrix = camera_comp->GetProjectionMatrix(aspect);
                }
            }
        }

        render_queue.Draw(graphics, camera_data);

        glfwSwapBuffers(m_window);
    }

}

void Engine::SetApplication(Application* app)
{
    m_application.reset(app);
}

Application* Engine::SetApplication()
{
    return m_application.get();
}

void Engine::SetCurrentScene(Scene* scene) {
    m_current_scene.reset(scene);
}

Scene* Engine::GetCurrentScene() const {
    return m_current_scene.get();
}

