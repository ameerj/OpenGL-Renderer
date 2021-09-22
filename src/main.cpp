#define GLFW_INCLUDE_NONE
#include <algorithm>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>

#include "common/common_types.h"
#include "model/model.h"
#include "renderer/renderer.h"
#include "shaders/shaders.h"

namespace {
float radius = 1, cam_height = 0, theta = 0.0f;

void ResetValues() {
    radius = 1;
    cam_height = 0;
    theta = 0.0f;
}

void OpenFile() {
    // From: https://iq.direct/blog/57-displaying-open-file-dialog-using-winapi.html
    OPENFILENAMEA ofn{0};
    char buffer[300];
    std::fill(buffer, buffer + 300, '\0');
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = 300;
    ofn.Flags = OFN_EXPLORER;
    ofn.lpstrFilter = NULL;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    std::cout << GetOpenFileNameA(&ofn) << std::endl;
    std::cout << buffer << (int)CommDlgExtendedError();
}

void ProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        // TODO: use this to dynamically load new models.
        OpenFile();
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        theta += 0.05f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        theta -= 0.05f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam_height += 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam_height -= 0.005f;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        radius += 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        radius -= 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        ResetValues();
    }
    radius = max(radius, 1.0f);
}
} // namespace

int main() {
    constexpr u32 width{1280};
    constexpr u32 height{720};
    auto* window{Renderer::InitWindow(width, height)};

    const Program quad_program{Shaders::GetRasterShader()};
    glUseProgram(quad_program.handle);

    Model::Model cube("test.obj");

    const glm::mat4 proj = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
    const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    const glm::vec3 translation = glm::vec3(0, 0, 0);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto eye = glm::vec3(radius * std::cos(theta), cam_height, radius * std::sin(theta));
        const auto at = glm::vec3(0.0, 0.0, 0.0);
        const auto up = glm::vec3(0.0, 1.0, 0.0);
        const auto modelViewMatrix = glm::lookAt(eye, at, up);
        const auto projectionMatrix = glm::perspective(60.0f, (f32)width / height, 0.01f, 10.0f);

        const auto mvp = projectionMatrix * modelViewMatrix;

        glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);

        glDrawElements(GL_TRIANGLES, cube.NumIndices(), GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(window);
        theta += 0.01f;
    }
    glfwTerminate();
    return 0;
}
