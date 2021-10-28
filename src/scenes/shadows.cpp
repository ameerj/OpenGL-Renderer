#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "shadows.h"

namespace Scenes {
void Shadows::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetPhongShader();
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

void Shadows::Render() {
    mesh_model.Render(GL_TRIANGLES);

    const auto eye = glm::vec3(camera_parameters.radius * std::cos(camera_parameters.theta),
                               camera_parameters.height,
                               camera_parameters.radius * std::sin(camera_parameters.theta));
    const auto at = glm::vec3(0.0, 0.0, 0.0);
    const auto up = glm::vec3(0.0, 1.0, 0.0);
    const auto view_matrix = glm::lookAt(eye, at, up);
    for (auto& mesh : walls) {
        const auto model_view_matrix = view_matrix * mesh.ModelMatrix();
        glUniformMatrix4fv(0, 1, GL_FALSE, &model_view_matrix[0][0]);
        mesh.Render(GL_TRIANGLES);
    }
}
} // namespace Scenes
