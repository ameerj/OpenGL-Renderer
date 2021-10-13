#define GLFW_INCLUDE_NONE
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer.h"

namespace Renderer {
namespace {
std::string_view NameOf(Scene scene) {
    switch (scene) {
    case Scene::Basic3D:
        return "Basic3D";
    case Scene::Phong3D:
        return "Phong3D";
    case Scene::TessBezier:
        return "TessBesizer";
    case Scene::GeomExplode:
        return "GeomExplode";
    default:
        return "UNKNOWN";
    }
}

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

        const auto eye = glm::vec3(camera_parameters.radius * std::cos(camera_parameters.theta),
                                   camera_parameters.height,
                                   camera_parameters.radius * std::sin(camera_parameters.theta));
        const auto at = glm::vec3(0.0, 0.0, 0.0);
        const auto up = glm::vec3(0.0, 1.0, 0.0);
        const auto view_matrix = glm::lookAt(eye, at, up);
        const auto model_view_matrix = view_matrix * model_matrix;
        GLenum topology = GL_TRIANGLES;

        switch (current_scene) {
        case Scene::Basic3D: {
            const auto mvp = projection_matrix * model_view_matrix;
            glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);
            break;
        }
        case Scene::TessBezier:
            glUniform1f(9, bezier_parameters.inner0);
            glUniform1f(10, bezier_parameters.inner1);
            glUniform1f(11, bezier_parameters.inner0);
            glUniform1f(12, bezier_parameters.inner1);

            glPatchParameteri(GL_PATCH_VERTICES, 16);

            topology = GL_PATCHES;
            [[fallthrough]];
        case Scene::Phong3D:
        case Scene::GeomExplode:
            glUniformMatrix4fv(0, 1, GL_FALSE, &model_view_matrix[0][0]);
            glUniformMatrix4fv(1, 1, GL_FALSE, &projection_matrix[0][0]);
            glUniform3f(2, 0.0f, 0.0f, 1.0f);

            glUniform3f(3, 0.0f, 0.0f, 0.0f);
            glUniform3f(4, 0.25f, 0.25f, 0.25f);
            glUniform1f(5, light_parameters.shininess);

            glUniform3fv(6, 1, &light_parameters.ambient[0]);
            glUniform3fv(7, 1, &light_parameters.diffuse[0]);
            glUniform3fv(8, 1, &light_parameters.specular[0]);
            break;
        default:
            break;
        }

        mesh_model.Render(topology);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Renderer::SetScene(Scene scene) {
    if (scene == current_scene) {
        return;
    }
    printf("Setting %s scene\n", NameOf(scene).data());
    current_scene = scene;
    const f32 aspect_ratio = (f32)window_width / window_height;

    switch (scene) {
    case Scene::Basic3D:
        SetMeshModel("../res/models/sonic.obj");
        shader_program = Shaders::GetRasterShader();
        glUseProgram(shader_program.handle);

        model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) *
                       glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

        projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.01f, 100.0f);
        glClearColor(0.25, 0.25, 0.25, 0.0);
        break;
    case Scene::Phong3D:
        SetMeshModel("../res/models/sonic.obj");
        shader_program = Shaders::GetPhongShader();
        glUseProgram(shader_program.handle);

        model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) *
                       glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

        projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.01f, 100.0f);
        glClearColor(0.25, 0.25, 0.25, 0.0);
        break;
    case Scene::TessBezier:
        shader_program = Shaders::GetBezierShader();
        SetBezierModel();
        glUseProgram(shader_program.handle);

        model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
                       glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 1, 0)) *
                       glm::translate(glm::mat4(1.0f), glm::vec3(-3, -3, 0));

        projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.01f, 100.0f);
        glClearColor(0.25, 0.25, 0.25, 0.0);
        break;
    case Scene::GeomExplode:
        SetMeshModel("../res/models/sonic.obj");
        shader_program = Shaders::GetGeometryShader();
        glUseProgram(shader_program.handle);

        model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) *
                       glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

        projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.01f, 100.0f);
        glClearColor(0.25, 0.25, 0.25, 0.0);
        break;
    default:
        break;
    }
    ResetParameters();
}

void Renderer::SetMeshModel(const std::string& path) {
    mesh_model.ParseObjModel(path);
}

void Renderer::SetBezierModel() {
    mesh_model.CreateBezierBuffers();
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
    glfwSetWindowUserPointer(window, this);
    auto func = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        static_cast<Renderer*>(glfwGetWindowUserPointer(window))
            ->KeyCallback(key, scancode, action, mods);
    };
    glfwSetKeyCallback(window, func);

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
void Renderer::ResetParameters() {
    camera_parameters.radius = 5;
    camera_parameters.height = 0;
    camera_parameters.theta = 0.0f;

    light_parameters.ambient = glm::vec3(0.5f);
    light_parameters.diffuse = glm::vec3(0.5f);
    light_parameters.specular = glm::vec3(0.5f);
}

void Renderer::KeyCallback(int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ResetParameters();
    }

    for (auto num_key = GLFW_KEY_1; num_key <= GLFW_KEY_9; ++num_key) {
        if (glfwGetKey(window, num_key) == GLFW_PRESS) {
            SetScene(static_cast<Scene>(num_key - GLFW_KEY_0));
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
} // namespace Renderer
