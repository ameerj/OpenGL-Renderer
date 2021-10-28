#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "basic_3d.h"

namespace Scenes {
void Basic3D::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetRasterShader();
    glUseProgram(shader_program.handle);
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
    const auto model_view_matrix = view_matrix * mesh_model.ModelMatrix();

    const auto mvp = projection_matrix * model_view_matrix;
    glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);
}

void Basic3D::Render() {
    mesh_model.Render(GL_TRIANGLES);
}
} // namespace Scenes
