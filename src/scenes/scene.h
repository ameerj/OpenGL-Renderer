#pragma once

#define GLFW_INCLUDE_NONE
#include <string_view>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>

#include "../model/model.h"
#include "../resources/resources.h"
#include "../shaders/shaders.h"

namespace Renderer {
class Renderer;
}

namespace Scenes {
class Scene {
public:
    Scene(GLFWwindow* window_, Renderer::Renderer& renderer_);
    virtual ~Scene() = default;
    virtual void Init() = 0;
    virtual void Configure() = 0;
    virtual void Render() = 0;

    virtual void ResetParameters();

    virtual void KeyCallback(int key, int scancode, int action, int mods);

    virtual std::string_view Name() {
        return "UNKNOWN";
    }

protected:
    struct CameraParameters {
        float radius = 5;
        float height{};
        float theta{};
    } camera_parameters{};

    GLFWwindow* window;
    Renderer::Renderer& renderer;

    Program shader_program;
    Model::Model mesh_model;

    glm::mat4 model_matrix;
    glm::mat4 projection_matrix;
};
} // namespace Scenes
