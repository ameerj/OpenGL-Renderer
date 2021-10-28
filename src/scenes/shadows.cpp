#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "shadows.h"

namespace Scenes {
void Shadows::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetMultiLightShader();
    glUseProgram(shader_program.handle);

    UpdateProjMtx();
    glClearColor(0.25, 0.25, 0.25, 0.0);

    const auto scale = glm::vec3(5.0f);

    // Back wall
    auto translate = glm::vec3(0.0f, -1.0f, 0.0f);
    auto rotate = glm::vec3(0.0f, 180.0f, 90.0f);
    walls[0].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);

    // Floor
    translate = glm::vec3(0.0f, -0.5, 0.0f);
    rotate = glm::vec3(0.0f, 0.0f, 0.0f);
    walls[1].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);

    // Right wall
    translate = glm::vec3(0.0f, -1.0f, 0.0f);
    rotate = glm::vec3(0.0f, 90.0, 90.0f);
    walls[2].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);

    // Left wall
    translate = glm::vec3(0.0f, -1.0f, 0.0f);
    rotate = glm::vec3(0.0f, 270.0, 90.0f);
    walls[3].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);
}

void Shadows::Configure() {
    glUniform3f(3, 0.0f, 0.0f, 0.0f);
    glUniform3f(4, 0.25f, 0.25f, 0.25f);
    glUniform1f(5, light_parameters.shininess);

    glUniform3fv(6, 1, &light_parameters.ambient[0]);
    glUniform3fv(7, 1, &light_parameters.diffuse[0]);
    glUniform3fv(8, 1, &light_parameters.specular[0]);
}

void Shadows::Render() {
    const auto eye = OrbitToWorldSpace(camera_parameters);
    const auto at = glm::vec3(0.0, 0.0, 0.0);
    const auto up = glm::vec3(0.0, 1.0, 0.0);
    const auto view_matrix = glm::lookAt(eye, at, up);
    const auto model_view_matrix = view_matrix * mesh_model.ModelMatrix();

    glUniformMatrix4fv(0, 1, GL_FALSE, &model_view_matrix[0][0]);
    glUniformMatrix4fv(1, 1, GL_FALSE, &projection_matrix[0][0]);

    const auto rotating_light_pos =
        model_view_matrix * glm::vec4(OrbitToWorldSpace(light_position), 1.0f);
    const std::array<float, 6> light_positions{
        0.0f, 0.0f, 1.0f, rotating_light_pos[0], rotating_light_pos[1], rotating_light_pos[2]};
    glUniform3fv(10, 2, light_positions.data());

    mesh_model.Render(GL_TRIANGLES);

    for (auto& mesh : walls) {
        const auto model_view_matrix = view_matrix * mesh.ModelMatrix();
        glUniformMatrix4fv(0, 1, GL_FALSE, &model_view_matrix[0][0]);
        mesh.Render(GL_TRIANGLES);
    }
}

void Shadows::KeyCallback(int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        light_position.height += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        light_position.height -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        light_position.theta += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        light_position.theta -= 0.1f;
    }
    Phong::KeyCallback(key, scancode, action, mods);
}
} // namespace Scenes
