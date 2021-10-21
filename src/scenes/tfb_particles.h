#pragma once
#include "scene.h"

namespace Scenes {
class TFBDemo : public Scene {
public:
    TFBDemo(GLFWwindow* window_, Renderer::Renderer& renderer_) : Scene(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void Render() override;

    void MouseCallback(GLFWwindow* window, int button, int action, int mods) override;

    std::string_view Name() override {
        return "TFBDemo";
    }

private:
    Buffer tfb_buffer{};
    std::unique_ptr<Mesh::Mesh> mesh;

    double mouse_x_pos{};
    double mouse_y_pos{};
    bool use_tfb_vbo{};
};
} // namespace Scenes
