#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "shadows.h"

namespace Scenes {
static constexpr u32 SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

void Shadows::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetMultiLightShader();
    shadow_shader_program = Shaders::GetShadowMappingShader();
    glClearColor(0.25, 0.25, 0.25, 0.0);

    UpdateProjMtx();
    CreateWalls();
    CreateDepthCubemap();
}

void Shadows::Configure() {}

void Shadows::Render() {
    light_world_pos = OrbitToWorldSpace(light_position);
    RenderShadowMap();

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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderer.GetWindowWidth(), renderer.GetWindowHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(2, 1, GL_FALSE, &projection_matrix[0][0]);
    const auto model_view_matrix = view_matrix * mesh_model.ModelMatrix();
    const auto rotating_light_transform = model_view_matrix * glm::vec4(light_world_pos, 1.0f);
    const std::array<float, 6> light_positions{
        eye.x, eye.y, eye.z, light_world_pos.x, light_world_pos.y, light_world_pos.z,
    };
    glUniform3fv(10, 2, light_positions.data());

    glActiveTexture(GL_TEXTURE1);
    glBindSampler(1, depth_sampler.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap.handle);
    RenderMeshes(true);
}

void Shadows::RenderMeshes(bool use_view_mtx) {
    const auto view_mtx = use_view_mtx ? view_matrix : glm::mat4(1.0);
    auto model_matrix = mesh_model.ModelMatrix();

    glUniformMatrix4fv(0, 1, GL_FALSE, &model_matrix[0][0]);
    glUniformMatrix4fv(1, 1, GL_FALSE, &view_mtx[0][0]);
    mesh_model.Render(GL_TRIANGLES);

    for (auto& mesh : walls) {
        model_matrix = mesh.ModelMatrix();
        glUniformMatrix4fv(0, 1, GL_FALSE, &model_matrix[0][0]);
        mesh.Render(GL_TRIANGLES);
    }
}

void Shadows::KeyCallback(int key, int scancode, int action, int mods) {
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

void Shadows::CreateWalls() {
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

void Shadows::CreateDepthCubemap() {
    depth_fbo.Create();
    depth_cubemap.Create();
    depth_sampler.Create();
    glBindSampler(1, depth_sampler.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap.handle);
    // Six faces for a cube map.
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
                     SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glSamplerParameteri(depth_sampler.handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(depth_sampler.handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(depth_sampler.handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(depth_sampler.handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Shadows::ConfigureDepthFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo.handle);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap.handle, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadows::RenderShadowMap() {
    ConfigureDepthFramebuffer();
    glUseProgram(shadow_shader_program.handle);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo.handle);
    glClear(GL_DEPTH_BUFFER_BIT);

    float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
    float near = 1.0f;
    float far = 25.0f;
    glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), aspect, near, far);

    std::vector<glm::mat4> shadow_matrices;
    shadow_matrices.push_back(shadow_proj * glm::lookAt(light_world_pos,
                                                        light_world_pos + glm::vec3(1.0, 0.0, 0.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadow_matrices.push_back(shadow_proj * glm::lookAt(light_world_pos,
                                                        light_world_pos + glm::vec3(-1.0, 0.0, 0.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadow_matrices.push_back(shadow_proj * glm::lookAt(light_world_pos,
                                                        light_world_pos + glm::vec3(0.0, 1.0, 0.0),
                                                        glm::vec3(0.0, 0.0, 1.0)));
    shadow_matrices.push_back(shadow_proj * glm::lookAt(light_world_pos,
                                                        light_world_pos + glm::vec3(0.0, -1.0, 0.0),
                                                        glm::vec3(0.0, .0, -1.0)));
    shadow_matrices.push_back(shadow_proj * glm::lookAt(light_world_pos,
                                                        light_world_pos + glm::vec3(0.0, 0.0, 1.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));
    shadow_matrices.push_back(shadow_proj * glm::lookAt(light_world_pos,
                                                        light_world_pos + glm::vec3(0.0, 0.0, -1.0),
                                                        glm::vec3(0.0, -1.0, 0.0)));

    glUniform3fv(2, 1, &light_world_pos[0]);
    glUniformMatrix4fv(16, 6, GL_FALSE, &shadow_matrices[0][0][0]);
    RenderMeshes(false);
}

} // namespace Scenes
