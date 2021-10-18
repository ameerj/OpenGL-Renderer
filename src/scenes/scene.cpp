#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "scene.h"

namespace Scenes {
Scene::Scene(GLFWwindow* window_, Renderer::Renderer& renderer_)
    : window{window_}, renderer{renderer_} {}

void Scene::UpdateProjMtx() {
    const f32 aspect_ratio = renderer.GetAspectRatio();
    projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.01f, 100.0f);
}

void Scene::ResetParameters() {
    camera_parameters.radius = 5;
    camera_parameters.height = 0;
    camera_parameters.theta = 0.0f;
}

void Scene::KeyCallback(int key, int scancode, int action, int mods) {
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
}
} // namespace Scenes
