#pragma once
#include <array>

#include "phong.h"

namespace Scenes {
class Shadows : public Phong {
public:
    Shadows(GLFWwindow* window_, Renderer::Renderer& renderer_) : Phong(window_, renderer_) {}

    void Init() override;

    void Render() override;

    std::string_view Name() override {
        return "Shadows";
    }

private:
    std::array<Model::Model, 4> walls{};
};
} // namespace Scenes
