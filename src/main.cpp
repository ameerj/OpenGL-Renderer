#define GLFW_INCLUDE_NONE
#include <algorithm>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "common/common_types.h"
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
    glUseProgram(quad_program.handle);

    const auto vertex_buffer{Shaders::GetVertexBuffer()};
    VAO vao;
    vao.Create();
    glBindVertexArray(vao.handle);

    glVertexArrayVertexBuffer(vao.handle, 0, vertex_buffer.handle, 0, 2 * sizeof(float));

    constexpr GLuint PositionLocation = 0;
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribFormat(PositionLocation, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(PositionLocation, 0);

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glUniform3f(2, color_red, color_green, color_blue);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        printf("Color: %.3f  %.3f   %.3f  \r", color_red, color_green, color_blue);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
