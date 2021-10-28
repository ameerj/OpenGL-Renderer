#pragma once
#include <array>

#include "phong.h"

namespace Scenes {
class Shadows : public Phong {
public:
    Shadows(GLFWwindow* window_, Renderer::Renderer& renderer_) : Phong(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void Render() override;

    virtual void KeyCallback(int key, int scancode, int action, int mods);

    std::string_view Name() override {
        return "Shadows";
    }

private:
    std::array<Model::Model, 4> walls{};

    OrbitingParameters light_position{
        .height = 0.0f,
        .theta = 1.0f,
    };
};
} // namespace Scenes
