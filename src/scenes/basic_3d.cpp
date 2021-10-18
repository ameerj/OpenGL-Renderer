#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "basic_3d.h"

namespace Scenes {
void Basic3D::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetRasterShader();
    glUseProgram(shader_program.handle);

    model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) *
                   glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
    UpdateProjMtx();
    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void Basic3D::Configure() {
    const auto eye = glm::vec3(camera_parameters.radius * std::cos(camera_parameters.theta),
                               camera_parameters.height,
                               camera_parameters.radius * std::sin(camera_parameters.theta));
    const auto at = glm::vec3(0.0, 0.0, 0.0);
    const auto up = glm::vec3(0.0, 1.0, 0.0);
    const auto view_matrix = glm::lookAt(eye, at, up);
    const auto model_view_matrix = view_matrix * model_matrix;
    const auto mvp = projection_matrix * model_view_matrix;
    glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);
}

void Basic3D::Render() {
    mesh_model.Render(GL_TRIANGLES);
}
} // namespace Scenes
