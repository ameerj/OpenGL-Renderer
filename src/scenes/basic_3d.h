#pragma once
#include "scene.h"

namespace Scenes {
class Basic3D : public Scene {
public:
    Basic3D(GLFWwindow* window_, Renderer::Renderer& renderer_) : Scene(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void Render() override;

    std::string_view Name() override {
        return "Basic3D";
    }
};
} // namespace Scenes
