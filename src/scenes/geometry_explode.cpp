#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "geometry_explode.h"

namespace Scenes {
void GeometryExplode::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetGeometryShader();
    glUseProgram(shader_program.handle);

    UpdateProjMtx();
    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void GeometryExplode::Configure() {
    Phong::Configure();
    glUniform1f(9, glfwGetTime());
}
} // namespace Scenes
