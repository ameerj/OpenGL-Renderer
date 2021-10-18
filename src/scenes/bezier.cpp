#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "bezier.h"

namespace Scenes {
void Bezier::Init() {
    shader_program = Shaders::GetBezierShader();
    mesh_model.CreateBezierBuffers();
    glUseProgram(shader_program.handle);

    model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
                   glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 1, 0)) *
                   glm::translate(glm::mat4(1.0f), glm::vec3(-3, -3, 0));

    UpdateProjMtx();
    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void Bezier::Configure() {
    Phong::Configure();

    glUniform1f(9, bezier_parameters.inner0);
    glUniform1f(10, bezier_parameters.inner1);
    glUniform1f(11, bezier_parameters.inner0);
    glUniform1f(12, bezier_parameters.inner1);
    if (using_geometry_shader) {
        glUniform1f(13, glfwGetTime());
    }
    glPatchParameteri(GL_PATCH_VERTICES, 16);
}

void Bezier::KeyCallback(int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        bezier_parameters.inner0 += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        bezier_parameters.inner0 -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        bezier_parameters.inner1 += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        bezier_parameters.inner1 -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
        shader_program = Shaders::GetBezierShader();
        glUseProgram(shader_program.handle);
        using_geometry_shader = false;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        shader_program = Shaders::GetGeometryBezierShader();
        glUseProgram(shader_program.handle);
        using_geometry_shader = true;
    }
    bezier_parameters.inner0 = glm::clamp(bezier_parameters.inner0, 2.0f, 100.0f);
    bezier_parameters.inner1 = glm::clamp(bezier_parameters.inner1, 2.0f, 100.0f);

    Scene::KeyCallback(key, scancode, action, mods);
}

void Bezier::ResetParameters() {
    Phong::ResetParameters();
    bezier_parameters = BezierParameters{
        .outer02{8.0f},
        .outer13{8.0f},
        .inner0{8.0f},
        .inner1{8.0f},
    };
}

void Bezier::Render() {
    mesh_model.Render(GL_PATCHES);
}
} // namespace Scenes
