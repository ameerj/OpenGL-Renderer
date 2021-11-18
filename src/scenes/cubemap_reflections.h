#pragma once
#include <array>

#include "phong.h"

namespace Scenes {
class CubemapReflections : public Phong {
public:
    CubemapReflections(GLFWwindow* window_, Renderer::Renderer& renderer_)
        : Phong(window_, renderer_) {}

    void Init() override;

    void Configure() override;

    void Render() override;

    virtual void KeyCallback(int key, int scancode, int action, int mods);

    std::string_view Name() override {
        return "CubemapReflections";
    }

private:
    void CreateWalls();

    void CreateCubemap();

    void ConfigureCubemapFramebuffer();

    void RenderCubeMap();

    void RenderColorScene();

    void RenderMirror();

    void RenderMeshes(bool use_view_mtx);

    std::array<Model::Model, 3> walls{};
    Model::Model back_wall{};

    OrbitingParameters light_position{
        .height = 0.0f,
        .theta = 1.0f,
    };

    glm::vec3 mirror_center{};
    glm::mat4 view_matrix{};

    Texture reflection_cubemap{};
    Texture depth_attachment{};
    Sampler reflection_sampler{};
    Framebuffer reflection_fbo{};

    Program cubemap_shader_program{};
    Program cubemap_sampling_program{};
};
} // namespace Scenes
