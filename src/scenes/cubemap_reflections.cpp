#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.h"
#include "cubemap_reflections.h"

namespace Scenes {
static constexpr u32 CUBEMAP_WIDTH = 1024, CUBEMAP_HEIGHT = 1024;

void CubemapReflections::Init() {
    mesh_model.ParseObjModel("../res/models/sonic.obj");
    shader_program = Shaders::GetMultiLightShader();
    cubemap_shader_program = Shaders::GetCubemapReflectionShader();
    glClearColor(0.25, 0.25, 0.25, 0.0);

    UpdateProjMtx();
    CreateWalls();
    CreateCubemap();

    // Hard code to be the back wall's center
    mirror_center = glm::vec3(1.0f, 0.0f, 0.0f);
}

void CubemapReflections::Configure() {}

void CubemapReflections::Render() {
    RenderCubeMap();

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
    const auto rotating_light_transform = model_view_matrix * glm::vec4(mirror_center, 1.0f);
    const std::array<float, 6> light_positions{
        eye.x, eye.y, eye.z, mirror_center.x, mirror_center.y, mirror_center.z,
    };
    glUniform3fv(10, 2, light_positions.data());

    glActiveTexture(GL_TEXTURE1);
    glBindSampler(1, reflection_sampler.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, reflection_cubemap.handle);
    RenderMeshes(true);
}

void CubemapReflections::RenderMeshes(bool use_view_mtx) {
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

void CubemapReflections::KeyCallback(int key, int scancode, int action, int mods) {
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

void CubemapReflections::CreateWalls() {
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

void CubemapReflections::CreateCubemap() {
    reflection_fbo.Create();
    depth_attachment.Create();
    reflection_cubemap.Create();
    reflection_sampler.Create();
    glBindSampler(1, reflection_sampler.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, reflection_cubemap.handle);
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, CUBEMAP_WIDTH,
                     CUBEMAP_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_attachment.handle);
    for (u32 i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, CUBEMAP_WIDTH,
                     CUBEMAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    reflection_sampler.DefaultConfiguration();
}

void CubemapReflections::ConfigureCubemapFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, reflection_fbo.handle);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflection_cubemap.handle, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_attachment.handle, 0);
}

void CubemapReflections::RenderCubeMap() {
    ConfigureCubemapFramebuffer();
    glUseProgram(cubemap_shader_program.handle);

    glViewport(0, 0, CUBEMAP_WIDTH, CUBEMAP_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (float)CUBEMAP_WIDTH / (float)CUBEMAP_HEIGHT;
    float near = 1.0f;
    float far = 25.0f;
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, near, far);

    std::vector<glm::mat4> proj_matrices;
    proj_matrices.push_back(proj * glm::lookAt(mirror_center,
                                               mirror_center + glm::vec3(1.0, 0.0, 0.0),
                                               glm::vec3(0.0, -1.0, 0.0)));
    proj_matrices.push_back(proj * glm::lookAt(mirror_center,
                                               mirror_center + glm::vec3(-1.0, 0.0, 0.0),
                                               glm::vec3(0.0, -1.0, 0.0)));
    proj_matrices.push_back(proj * glm::lookAt(mirror_center,
                                               mirror_center + glm::vec3(0.0, 1.0, 0.0),
                                               glm::vec3(0.0, 0.0, 1.0)));
    proj_matrices.push_back(proj * glm::lookAt(mirror_center,
                                               mirror_center + glm::vec3(0.0, -1.0, 0.0),
                                               glm::vec3(0.0, .0, -1.0)));
    proj_matrices.push_back(proj * glm::lookAt(mirror_center,
                                               mirror_center + glm::vec3(0.0, 0.0, 1.0),
                                               glm::vec3(0.0, -1.0, 0.0)));
    proj_matrices.push_back(proj * glm::lookAt(mirror_center,
                                               mirror_center + glm::vec3(0.0, 0.0, -1.0),
                                               glm::vec3(0.0, -1.0, 0.0)));

    glUniform3fv(2, 1, &mirror_center[0]);
    glUniformMatrix4fv(16, 6, GL_FALSE, &proj_matrices[0][0][0]);

    glUniform3f(3, 0.0f, 0.0f, 0.0f);
    glUniform3f(4, 0.25f, 0.25f, 0.25f);
    glUniform1f(5, light_parameters.shininess);

    glUniform3fv(6, 1, &light_parameters.ambient[0]);
    glUniform3fv(7, 1, &light_parameters.diffuse[0]);
    glUniform3fv(8, 1, &light_parameters.specular[0]);

    RenderMeshes(false);
}

} // namespace Scenes
