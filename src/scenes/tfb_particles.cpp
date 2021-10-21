#include <algorithm>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../model/mesh.h"
#include "../renderer/renderer.h"
#include "tfb_particles.h"

namespace Scenes {
namespace {
float RandomNormalized() {
    float min = -1;
    float max = 1;
    float r = (float)rand() / (float)RAND_MAX;
    float result = min + r * (max - min);
    return result;
}

Vertex Random2DNormalized() {
    return {{RandomNormalized(), RandomNormalized(), 0.0}};
}
} // namespace

void TFBDemo::Init() {
    std::vector<Vertex> points(20000);
    std::generate(points.begin(), points.end(), Random2DNormalized);
    mesh = std::make_unique<Mesh::Mesh>(points);
    shader_program = Shaders::GetTfbShader();
    glUseProgram(shader_program.handle);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    tfb_buffer.Create();
    glBindBuffer(GL_ARRAY_BUFFER, tfb_buffer.handle);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(points[0]), nullptr,
                 GL_STATIC_DRAW | GL_STATIC_READ);
}

void TFBDemo::Configure() {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glUniform2f(0, mouse_x_pos, mouse_y_pos);

    glUniform1ui(1, invert_gravity ? 1u : 0u);

    const auto tfb_handle = use_tfb_vbo ? mesh->VertexBuffer() : tfb_buffer.handle;
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfb_handle);
}

void TFBDemo::Render() {
    glBeginTransformFeedback(GL_POINTS);
    const auto vbo_handle = use_tfb_vbo ? tfb_buffer.handle : mesh->VertexBuffer();
    mesh->Render(GL_POINTS, vbo_handle);
    glEndTransformFeedback();

    use_tfb_vbo = !use_tfb_vbo;
}

void TFBDemo::MouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action != GLFW_PRESS) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        glfwGetCursorPos(window, &mouse_x_pos, &mouse_y_pos);
        const auto window_width = static_cast<double>(renderer.GetWindowWidth());
        const auto window_height = static_cast<double>(renderer.GetWindowHeight());
        mouse_x_pos = 2.0 * (mouse_x_pos / window_width) - 1.0;
        mouse_y_pos = 2.0 * (1.0 - (mouse_y_pos / window_height)) - 1.0;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        invert_gravity = !invert_gravity;
    }
}

} // namespace Scenes
