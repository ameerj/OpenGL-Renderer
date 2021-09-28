#pragma once

#include "../common/common_types.h"
#include "../model/model.h"
#include "../shaders/shaders.h"

namespace Renderer {
class Renderer {
public:
    Renderer(u32 width, u32 height);
    ~Renderer();

    void RenderLoop();

    void SetMeshModel(const std::string& path);

private:
    void InitWindow();
    void ResetCameraParameters();
    void ProcessInput();

    u32 window_width{};
    u32 window_height{};

    struct CameraParameters {
        float radius = 5;
        float height{};
        float theta{};
    } camera_parameters{};

    GLFWwindow* window;
    Model::Model mesh_model;
    Program shader_program;
};
} // namespace Renderer
