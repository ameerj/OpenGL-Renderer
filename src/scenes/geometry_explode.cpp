#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "geometry_explode.h"

namespace Scenes {
void GeometryExplode::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetGeometryShader();
    glUseProgram(shader_program.handle);

    model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) *
                   glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

    projection_matrix =
        glm::perspective(glm::radians(45.0f), renderer.GetAspectRatio(), 0.01f, 100.0f);
    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void GeometryExplode::Configure() {
    Phong::Configure();
    glUniform1f(9, glfwGetTime());
}
} // namespace Scenes
