#pragma once
#include <array>

#include "phong.h"

namespace Scenes {
class Reflections : public Phong {
public:
    Reflections(GLFWwindow* window_, Renderer::Renderer& renderer_) : Phong(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void Render() override;

    virtual void KeyCallback(int key, int scancode, int action, int mods);

    std::string_view Name() override {
        return "Reflections";
    }

private:
    void CreateWalls();

    void CreateFrameTexture();

    void ConfigureFramebuffer();

    void RenderSSRTextures();

    void RenderSSRRayMarch();

    void RenderMeshes();

    void DrawFullscreenTexture();

    std::array<Model::Model, 3> walls{};
    Model::Model floor{};

    OrbitingParameters light_position{
        .height = 0.0f,
        .theta = 1.0f,
    };

    glm::vec3 light_world_pos{};
    glm::mat4 view_matrix{};

    Texture color_attachment{};
    Texture normals_attachment{};
    Texture positions_attachment{};
    Sampler color_sampler{};
    Texture depth_attachment{};
    Sampler depth_sampler{};
    Framebuffer frame_fbo{};

    Program ray_march_shader_program{};
    Program fullscreen_shader_program{};
};
} // namespace Scenes
