#pragma once

#include <glm/mat4x4.hpp>

#include "../common/common_types.h"
#include "../model/model.h"
#include "../shaders/shaders.h"

namespace Renderer {
enum class Scene : u8 {
    UNKNOWN,
    Basic3D,
    Phong3D,
    TessBezier,
};

class Renderer {
public:
    Renderer(u32 width, u32 height);
    ~Renderer();

    void RenderLoop();

    void SetScene(Scene scene);

    void SetMeshModel(const std::string& path);

    void SetBezierModel();

private:
    void InitWindow();
    void ResetParameters();
    void ProcessInput();

    u32 window_width{};
    u32 window_height{};

    struct CameraParameters {
        float radius = 5;
        float height{};
        float theta{};
    } camera_parameters{};

    struct LightParameters {
        glm::vec3 ambient{0.5f};
        glm::vec3 diffuse{0.5f};
        glm::vec3 specular{0.5f};
        float shininess{20.0f};
    } light_parameters;

    struct BezierParameters {
        float outer02{10.0f};
        float outer13{10.0f};
        float inner0{10.0f};
        float inner1{10.0f};
    } bezier_parameters;

    GLFWwindow* window;
    Model::Model mesh_model;
    Program shader_program;

    glm::mat4 model_matrix;
    glm::mat4 projection_matrix;

    Scene current_scene = Scene::UNKNOWN;
};
} // namespace Renderer
