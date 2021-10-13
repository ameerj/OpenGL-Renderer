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

    projection_matrix =
        glm::perspective(glm::radians(45.0f), renderer.GetAspectRatio(), 0.01f, 100.0f);
    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void Bezier::Configure() {
    Phong::Configure();

    glUniform1f(9, bezier_parameters.inner0);
    glUniform1f(10, bezier_parameters.inner1);
    glUniform1f(11, bezier_parameters.inner0);
    glUniform1f(12, bezier_parameters.inner1);

    glPatchParameteri(GL_PATCH_VERTICES, 16);
}

void Bezier::KeyCallback(int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ResetParameters();
    }

    for (auto num_key = GLFW_KEY_1; num_key <= GLFW_KEY_9; ++num_key) {
        if (glfwGetKey(window, num_key) == GLFW_PRESS) {
            renderer.SetScene(static_cast<Renderer::SceneName>(num_key - GLFW_KEY_0));
            return;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera_parameters.theta += 0.05f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_parameters.theta -= 0.05f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_parameters.height += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_parameters.height -= 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera_parameters.radius += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_parameters.radius -= 0.1f;
    }
    camera_parameters.radius = std::max(camera_parameters.radius, 1.0f);
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
    bezier_parameters.inner0 = glm::clamp(bezier_parameters.inner0, 2.0f, 20.0f);
    bezier_parameters.inner1 = glm::clamp(bezier_parameters.inner1, 2.0f, 20.0f);
}

void Bezier::Render() {
    mesh_model.Render(GL_PATCHES);
}
} // namespace Scenes
