#pragma once
#include "scene.h"

namespace Scenes {
class TFBDemo : public Scene {
public:
    TFBDemo(GLFWwindow* window_, Renderer::Renderer& renderer_) : Scene(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void Render() override;

    std::string_view Name() override {
        return "TFBDemo";
    }
};
} // namespace Scenes
