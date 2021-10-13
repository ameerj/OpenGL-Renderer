#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "phong.h"

namespace Scenes {
void Phong::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetPhongShader();
    glUseProgram(shader_program.handle);

    model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) *
                   glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

    projection_matrix =
        glm::perspective(glm::radians(45.0f), renderer.GetAspectRatio(), 0.01f, 100.0f);
    glClearColor(0.25, 0.25, 0.25, 0.0);
}

void Phong::Configure() {
    const auto eye = glm::vec3(camera_parameters.radius * std::cos(camera_parameters.theta),
                               camera_parameters.height,
                               camera_parameters.radius * std::sin(camera_parameters.theta));
    const auto at = glm::vec3(0.0, 0.0, 0.0);
    const auto up = glm::vec3(0.0, 1.0, 0.0);
    const auto view_matrix = glm::lookAt(eye, at, up);
    const auto model_view_matrix = view_matrix * model_matrix;
    glUniformMatrix4fv(0, 1, GL_FALSE, &model_view_matrix[0][0]);
    glUniformMatrix4fv(1, 1, GL_FALSE, &projection_matrix[0][0]);
    glUniform3f(2, 0.0f, 0.0f, 1.0f);

    glUniform3f(3, 0.0f, 0.0f, 0.0f);
    glUniform3f(4, 0.25f, 0.25f, 0.25f);
    glUniform1f(5, light_parameters.shininess);

    glUniform3fv(6, 1, &light_parameters.ambient[0]);
    glUniform3fv(7, 1, &light_parameters.diffuse[0]);
    glUniform3fv(8, 1, &light_parameters.specular[0]);
}

void Phong::Render() {
    mesh_model.Render(GL_TRIANGLES);
}

void Phong::ResetParameters() {
    Scene::ResetParameters();
    light_parameters.ambient = glm::vec3(0.5f);
    light_parameters.diffuse = glm::vec3(0.5f);
    light_parameters.specular = glm::vec3(0.5f);
}

void Phong::KeyCallback(int key, int scancode, int action, int mods) {
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
    camera_parameters.radius = glm::max(camera_parameters.radius, 1.0f);

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        light_parameters.diffuse += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        light_parameters.diffuse -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        light_parameters.specular += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        light_parameters.specular -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        light_parameters.ambient += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        light_parameters.ambient -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        light_parameters.shininess += 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        light_parameters.shininess -= 0.5f;
    }
    const auto zero_vec3 = glm::vec3(0.0f);
    const auto one_vec3 = glm::vec3(1.0f);
    light_parameters.diffuse = glm::clamp(light_parameters.diffuse, zero_vec3, one_vec3);
    light_parameters.ambient = glm::clamp(light_parameters.ambient, zero_vec3, one_vec3);
    light_parameters.specular = glm::clamp(light_parameters.specular, zero_vec3, one_vec3);
    light_parameters.shininess = glm::clamp(light_parameters.shininess, 1.0f, 1000.0f);
}
} // namespace Scenes