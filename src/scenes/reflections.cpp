#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "Reflections.h"

namespace Scenes {
static constexpr u32 SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

void Reflections::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetMultiLightShader();
    fullscreen_shader_program = Shaders::GetFullscreenShader();
    glUseProgram(shader_program.handle);
    glClearColor(0.25, 0.25, 0.25, 0.0);

    UpdateProjMtx();
    CreateWalls();
    CreateFrameTexture();
}

void Reflections::Configure() {}

void Reflections::Render() {
    glUseProgram(shader_program.handle);

    const auto eye = OrbitToWorldSpace(camera_parameters);
    const auto at = glm::vec3(0.0, 0.0, 0.0);
    const auto up = glm::vec3(0.0, 1.0, 0.0);
    view_matrix = glm::lookAt(eye, at, up);

    glUniform3f(3, 0.0f, 0.0f, 0.0f);
    glUniform3f(4, 0.25f, 0.25f, 0.25f);
    glUniform1f(5, light_parameters.shininess);

    glUniform3fv(6, 1, &light_parameters.ambient[0]);
    glUniform3fv(7, 1, &light_parameters.diffuse[0]);
    glUniform3fv(8, 1, &light_parameters.specular[0]);

    glUniform3fv(9, 1, &eye[0]);

    ConfigureFramebuffer();
    glViewport(0, 0, renderer.GetWindowWidth(), renderer.GetWindowHeight());

    glUniformMatrix4fv(2, 1, GL_FALSE, &projection_matrix[0][0]);
    const auto model_view_matrix = view_matrix * mesh_model.ModelMatrix();
    const auto rotating_light_transform = model_view_matrix * glm::vec4(light_world_pos, 1.0f);
    const std::array<float, 6> light_positions{
        eye.x, eye.y, eye.z, light_world_pos.x, light_world_pos.y, light_world_pos.z,
    };
    glUniform3fv(10, 2, light_positions.data());

    RenderMeshes();

    DrawFullscreenTexture();
}

void Reflections::RenderMeshes() {
    const auto view_mtx = view_matrix;
    glUniformMatrix4fv(1, 1, GL_FALSE, &view_mtx[0][0]);

    for (auto& mesh : walls) {
        auto model_matrix = mesh.ModelMatrix();
        glUniformMatrix4fv(0, 1, GL_FALSE, &model_matrix[0][0]);
        mesh.Render(GL_TRIANGLES);
    }
    auto model_matrix = mesh_model.ModelMatrix();
    glUniformMatrix4fv(0, 1, GL_FALSE, &model_matrix[0][0]);
    mesh_model.Render(GL_TRIANGLES);
}

void Reflections::DrawFullscreenTexture() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(fullscreen_shader_program.handle);

    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, color_sampler.handle);
    glBindTexture(GL_TEXTURE_2D, color_attachment.handle);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Reflections::KeyCallback(int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        light_position.height += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        light_position.height -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        light_position.theta += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        light_position.theta -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        light_position.radius += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        light_position.radius -= 0.1f;
    }
    light_position.radius = glm::max(0.5f, light_position.radius);
    Phong::KeyCallback(key, scancode, action, mods);
}

void Reflections::CreateWalls() {
    const auto scale = glm::vec3(5.0f);

    // Back wall
    auto translate = glm::vec3(0.0f, -1.0f, 0.0f);
    auto rotate = glm::vec3(0.0f, 180.0f, 90.0f);
    walls[0].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);

    // Floor
    translate = glm::vec3(0.0f, -0.5, 0.0f);
    rotate = glm::vec3(0.0f, 0.0f, 0.0f);
    walls[1].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);

    // Right wall
    translate = glm::vec3(0.0f, -1.0f, 0.0f);
    rotate = glm::vec3(0.0f, 90.0, 90.0f);
    walls[2].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);

    // Left wall
    translate = glm::vec3(0.0f, -1.0f, 0.0f);
    rotate = glm::vec3(0.0f, 270.0, 90.0f);
    walls[3].ParseObjModel("../res/models/plane.obj", scale, translate, rotate);
}

void Reflections::CreateFrameTexture() {
    color_attachment.Create();
    color_sampler.Create();
    depth_attachment.Create();
    depth_sampler.Create();
    frame_fbo.Create();

    const auto window_width = renderer.GetWindowWidth();
    const auto window_height = renderer.GetWindowHeight();
    glBindTexture(GL_TEXTURE_2D, color_attachment.handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 nullptr);
    glBindTexture(GL_TEXTURE_2D, depth_attachment.handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, window_width, window_height, 0,
                 GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    color_sampler.DefaultConfiguration();
}

void Reflections::ConfigureFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, frame_fbo.handle);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_attachment.handle, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_attachment.handle, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void Reflections::RenderShadowMap() {
    ConfigureFramebuffer();
}

} // namespace Scenes
