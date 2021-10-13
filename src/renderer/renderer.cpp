#define GLFW_INCLUDE_NONE
#include <iostream>
#include <memory>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../scenes/basic_3d.h"
#include "../scenes/bezier.h"
#include "../scenes/geometry_explode.h"
#include "../scenes/phong.h"
#include "../scenes/scene.h"
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
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->Configure();
        scene->Render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Renderer::SetScene(SceneName scene_name) {
    if (scene_name == current_scene) {
        return;
    }
    switch (scene_name) {
    case SceneName::Basic3D:
        scene = std::make_unique<Scenes::Basic3D>(window, *this);
        break;
    case SceneName::Phong3D:
        scene = std::make_unique<Scenes::Phong>(window, *this);
        break;
    case SceneName::TessBezier:
        scene = std::make_unique<Scenes::Bezier>(window, *this);
        break;
    case SceneName::GeomExplode:
        scene = std::make_unique<Scenes::GeometryExplode>(window, *this);
        break;
    default:
        return;
    }
    current_scene = scene_name;
    scene->Init();
    printf("Setting %s scene\n", scene->Name().data());
    glfwSetWindowUserPointer(window, scene.get());
    auto func = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        static_cast<Scenes::Scene*>(glfwGetWindowUserPointer(window))
            ->KeyCallback(key, scancode, action, mods);
    };
    glfwSetKeyCallback(window, func);
}

float Renderer::GetAspectRatio() {
    return (f32)window_width / window_height;
}

void Renderer::InitWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
} // namespace Renderer
