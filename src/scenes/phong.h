#pragma once
#include "scene.h"

namespace Scenes {
class Phong : public Scene {
public:
    Phong(GLFWwindow* window_, Renderer::Renderer& renderer_) : Scene(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void Render() override;

    void ResetParameters() override;

    void KeyCallback(int key, int scancode, int action, int mods) override;

    std::string_view Name() override {
        return "Phong3D";
    }

protected:
    struct LightParameters {
        glm::vec3 ambient{0.5f};
        glm::vec3 diffuse{0.5f};
        glm::vec3 specular{0.5f};
        float shininess{20.0f};
    } light_parameters;
};
} // namespace Scenes
