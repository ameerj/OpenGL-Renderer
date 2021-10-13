#pragma once
#include "phong.h"

namespace Scenes {
class GeometryExplode : public Phong {
public:
    GeometryExplode(GLFWwindow* window_, Renderer::Renderer& renderer_)
        : Phong(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    std::string_view Name() override {
        return "GeometryExplode";
    }
};
} // namespace Scenes
