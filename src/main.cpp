#define GLFW_INCLUDE_NONE
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "common/common_types.h"
#include "model/mesh_model.h"
#include "renderer/renderer.h"
#include "shaders/shaders.h"

namespace {
float color_red = 1.0f;
float color_green = 0.0f;
float color_blue = 0.0f;

void ResetValues() {
    color_red = 1.0f;
    color_green = 0.0f;
    color_blue = 0.0f;
}

void ProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        color_red += 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        color_red -= 0.005f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        color_green += 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        color_green -= 0.005f;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        color_blue += 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        color_blue -= 0.005f;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        ResetValues();
    }
    color_red = std::clamp(color_red, 0.0f, 1.0f);
    color_green = std::clamp(color_green, 0.0f, 1.0f);
    color_blue = std::clamp(color_blue, 0.0f, 1.0f);
}
} // namespace

int main() {
    constexpr u32 width{1280};
    constexpr u32 height{720};
    auto* window{Renderer::InitWindow(width, height)};

    const Program quad_program{Shaders::GetRasterShader()};
    const auto vertex_array_object{Shaders::BindQuadBuffers()};

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(quad_program.handle);
        glUniform3f(2, color_red, color_green, color_blue);
        glBindVertexArray(vertex_array_object);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        printf("color: %.3f  %.3f   %.3f  \r", color_red, color_green, color_blue);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
