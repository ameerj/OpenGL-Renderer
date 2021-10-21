#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    std::vector<Vertex> points(3000);
    std::generate(points.begin(), points.end(), Random2DNormalized);
    mesh_model.AddMesh(points);
    shader_program = Shaders::GetTfbShader();
    glUseProgram(shader_program.handle);

    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void TFBDemo::Configure() {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glUniform2f(0, mouse_x_pos, mouse_y_pos);
}

void TFBDemo::Render() {
    mesh_model.Render(GL_POINTS);
}

void TFBDemo::MouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        glfwGetCursorPos(window, &mouse_x_pos, &mouse_y_pos);
        mouse_x_pos = 2.0 * (mouse_x_pos / static_cast<double>(renderer.GetWindowWidth())) - 1.0;
        mouse_y_pos =
            2.0 * (1.0 - (mouse_y_pos / static_cast<double>(renderer.GetWindowHeight()))) - 1.0;
        printf("%lf, %lf\n", mouse_x_pos, mouse_y_pos);
    }
}

} // namespace Scenes
