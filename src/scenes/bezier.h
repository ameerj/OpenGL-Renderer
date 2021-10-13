#pragma once
#include "phong.h"

namespace Scenes {
class Bezier : public Phong {
public:
    Bezier(GLFWwindow* window_, Renderer::Renderer& renderer_) : Phong(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void KeyCallback(int key, int scancode, int action, int mods) override;

    void Render() override;

    std::string_view Name() override {
        return "BezierTess";
    }

private:
    struct BezierParameters {
        float outer02{10.0f};
        float outer13{10.0f};
        float inner0{10.0f};
        float inner1{10.0f};
    } bezier_parameters;
};
} // namespace Scenes
