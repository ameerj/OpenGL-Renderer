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
    void CreateWalls();

    void CreateDepthCubemap();

    void ConfigureDepthFramebuffer();

    void RenderShadowMap();

    void RenderMeshes(bool use_view_mtx);

    std::array<Model::Model, 4> walls{};

    OrbitingParameters light_position{
        .height = 0.0f,
        .theta = 1.0f,
    };

    glm::vec3 light_world_pos{};
    glm::mat4 view_matrix{};

    Texture depth_cubemap{};
    Sampler depth_sampler{};
    Framebuffer depth_fbo{};
    Program shadow_shader_program{};
};
} // namespace Scenes
