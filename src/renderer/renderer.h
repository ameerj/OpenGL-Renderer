#pragma once

#include <memory>

#include <glm/mat4x4.hpp>

#include "../common/common_types.h"

struct GLFWwindow;

namespace Scenes {
class Scene;
}

namespace Renderer {
enum class SceneName : u8 {
    UNKNOWN,
    Basic3D,
    Phong3D,
    TessBezier,
    GeomExplode,
    TFBDemo,
    Shadows,
};

class Renderer {
public:
    Renderer(u32 width, u32 height);
    ~Renderer();

    void RenderLoop();

    void SetScene(SceneName scene);

    float GetAspectRatio();

    u32 GetWindowWidth() {
        return window_width;
    }

    u32 GetWindowHeight() {
        return window_height;
    }

private:
    void InitWindow();

    void KeyCallback(int key, int scancode, int action, int mods);

    void MouseCallback(GLFWwindow* window, int button, int action, int mods);

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    u32 window_width{};
    u32 window_height{};

    GLFWwindow* window;

    SceneName current_scene = SceneName::UNKNOWN;
    std::unique_ptr<Scenes::Scene> scene{nullptr};
};
} // namespace Renderer
