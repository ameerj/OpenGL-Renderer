#define GLFW_INCLUDE_NONE
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer.h"

namespace Renderer {
namespace {
const char* GetSource(GLenum source) {
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
        return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
        return "OTHER";
    default:
        return "Unknown source";
    }
}

const char* GetType(GLenum type) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
        return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
        return "PERFORMANCE";
    case GL_DEBUG_TYPE_OTHER:
        return "OTHER";
    case GL_DEBUG_TYPE_MARKER:
        return "MARKER";
    default:
        return "Unknown type";
    }
}

void FbSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar* message, const void* userParam) {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        fprintf(stderr, "%s %s 0x%x: %s\n", GetSource(source), GetType(type), id, message);
        break;
    default:
        // TODO: print other severities if oddities encountered.
        break;
    }
}
} // namespace

Renderer::Renderer(u32 width, u32 height) : window_width{width}, window_height{height} {
    InitWindow();
}

Renderer::~Renderer() {
    glfwTerminate();
}

void Renderer::RenderLoop() {
    shader_program = Shaders::GetRasterShader();
    glUseProgram(shader_program.handle);

    glEnable(GL_DEPTH_TEST);

    const auto model_matrix =
        glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

    const glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    const glm::vec3 translation = glm::vec3(0, 0, 0);

    const auto projection_matrix =
        glm::perspective(glm::radians(45.0f), (f32)window_width / window_height, 0.01f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        ProcessInput();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto eye = glm::vec3(camera_parameters.radius * std::cos(camera_parameters.theta),
                                   camera_parameters.height,
                                   camera_parameters.radius * std::sin(camera_parameters.theta));
        const auto at = glm::vec3(0.0, 0.0, 0.0);
        const auto up = glm::vec3(0.0, 1.0, 0.0);
        const auto view_matrix = glm::lookAt(eye, at, up);
        const auto model_view_matrix = view_matrix * model_matrix;

        const auto mvp = projection_matrix * model_view_matrix;

        glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);

        mesh_model.Render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Renderer::SetMeshModel(const std::string& path) {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
}

void Renderer::InitWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(window_width, window_height, "OpenGL Renderer", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FbSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugMessage, NULL);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}
void Renderer::ResetCameraParameters() {
    camera_parameters.radius = 5;
    camera_parameters.height = 0;
    camera_parameters.theta = 0.0f;
}

void Renderer::ProcessInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
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
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        ResetCameraParameters();
    }
    camera_parameters.radius = std::max(camera_parameters.radius, 1.0f);
}
} // namespace Renderer
