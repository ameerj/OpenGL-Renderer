#define GLFW_INCLUDE_NONE
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "../common/common_types.h"
#include "basic_rasterizer.h"
#include "bezier_geom.h"
#include "bezier_tess.h"
#include "geometry_shader.h"
#include "phong_shading.h"
#include "shaders.h"
#include "shadow_mapping.h"
#include "tfb_particles.h"
#include "two_light_shading.h"

namespace Shaders {
namespace {
enum ShaderStage {
    VERTEX_STAGE = 0,
    TESS_CONTROL_STAGE,
    TESS_EVALUATION_STAGE,
    GEOMETRY_STAGE,
    FRAGMENT_STAGE,
};
constexpr size_t NUM_STAGES = 5;

GLenum GetGlShaderStage(ShaderStage stage) {
    switch (stage) {
    case ShaderStage::VERTEX_STAGE:
        return GL_VERTEX_SHADER;
    case ShaderStage::TESS_CONTROL_STAGE:
        return GL_TESS_CONTROL_SHADER;
    case ShaderStage::TESS_EVALUATION_STAGE:
        return GL_TESS_EVALUATION_SHADER;
    case ShaderStage::GEOMETRY_STAGE:
        return GL_GEOMETRY_SHADER;
    case ShaderStage::FRAGMENT_STAGE:
        return GL_FRAGMENT_SHADER;
    }
}

Shader CompileShader(GLenum stage, std::string_view shader_src) {
    Shader shader;
    shader.handle = glCreateShader(stage);
    const GLchar* const code_ptr = shader_src.data();
    glShaderSource(shader.handle, 1, &code_ptr, NULL);
    glCompileShader(shader.handle);

    return shader;
}

void LinkProgram(GLuint handle) {
    glLinkProgram(handle);

    GLint link_status{};
    glGetProgramiv(handle, GL_LINK_STATUS, &link_status);

    GLint log_length{};
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length <= 1) {
        return;
    }
    std::string log(log_length, 0);
    glGetProgramInfoLog(handle, log_length, nullptr, log.data());
    if (link_status == GL_FALSE) {
        fprintf(stderr, "LINK ERROR: %s\n", log.c_str());
    } else {
        fprintf(stderr, "LINK LOG: %s\n", log.c_str());
    }
}

Program CreateShaderProgram(const std::array<std::string_view, NUM_STAGES>& shader_code) {
    std::vector<Shader> shaders;
    for (size_t stage = 0; stage < shader_code.size(); ++stage) {
        auto code = shader_code[stage];
        if (code.empty()) {
            continue;
        }
        const auto shader_stage = static_cast<ShaderStage>(stage);
        shaders.push_back(CompileShader(GetGlShaderStage(shader_stage), code));
    }
    Program program;
    program.handle = glCreateProgram();
    for (const auto& shader : shaders) {
        glAttachShader(program.handle, shader.handle);
    }
    LinkProgram(program.handle);
    return program;
}
} // namespace

Program GetRasterShader() {
    std::array<std::string_view, NUM_STAGES> shader_code{};
    shader_code[ShaderStage::VERTEX_STAGE] = basic_vert;
    shader_code[ShaderStage::FRAGMENT_STAGE] = basic_frag;

    return CreateShaderProgram(shader_code);
}

Program GetPhongShader() {
    std::array<std::string_view, NUM_STAGES> shader_code{};
    shader_code[ShaderStage::VERTEX_STAGE] = phong_vert;
    shader_code[ShaderStage::FRAGMENT_STAGE] = phong_frag;

    return CreateShaderProgram(shader_code);
}

Program GetBezierShader() {
    std::array<std::string_view, NUM_STAGES> shader_code{};
    shader_code[ShaderStage::VERTEX_STAGE] = bezier_vert;
    shader_code[ShaderStage::TESS_CONTROL_STAGE] = bezier_tcs;
    shader_code[ShaderStage::TESS_EVALUATION_STAGE] = bezier_tes;
    shader_code[ShaderStage::FRAGMENT_STAGE] = bezier_frag;

    return CreateShaderProgram(shader_code);
}

Program GetGeometryShader() {
    std::array<std::string_view, NUM_STAGES> shader_code{};
    shader_code[ShaderStage::VERTEX_STAGE] = explode_vert;
    shader_code[ShaderStage::GEOMETRY_STAGE] = explode_geom;
    shader_code[ShaderStage::FRAGMENT_STAGE] = explode_frag;

    return CreateShaderProgram(shader_code);
}

Program GetGeometryBezierShader() {
    std::array<std::string_view, NUM_STAGES> shader_code{};
    shader_code[ShaderStage::VERTEX_STAGE] = bezier_vert;
    shader_code[ShaderStage::TESS_CONTROL_STAGE] = bezier_tcs;
    shader_code[ShaderStage::TESS_EVALUATION_STAGE] = bezier_tes;
    shader_code[ShaderStage::GEOMETRY_STAGE] = bezier_geom;
    shader_code[ShaderStage::FRAGMENT_STAGE] = bezier_frag;

    return CreateShaderProgram(shader_code);
}

Program GetTfbShader() {
    const char* vertex_shader_code = tfb_vert.data();
    const char* fragment_shader_code = tfb_frag.data();

    const Shader vertex = CompileShader(GL_VERTEX_SHADER, vertex_shader_code);
    const Shader fragment = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    Program program;
    program.handle = glCreateProgram();

    glAttachShader(program.handle, vertex.handle);
    glAttachShader(program.handle, fragment.handle);

    const GLchar* tfb_attrs[] = {"transformed_coord", "tfb_normal", "tfb_tex_coord"};
    glTransformFeedbackVaryings(program.handle, 3, tfb_attrs, GL_INTERLEAVED_ATTRIBS);

    LinkProgram(program.handle);

    return program;
}

Program GetMultiLightShader() {
    std::array<std::string_view, NUM_STAGES> shader_code{};
    shader_code[ShaderStage::VERTEX_STAGE] = two_light_vert;
    shader_code[ShaderStage::FRAGMENT_STAGE] = two_light_frag;

    return CreateShaderProgram(shader_code);
}

Program GetShadowMappingShader() {
    std::array<std::string_view, NUM_STAGES> shader_code{};
    shader_code[ShaderStage::VERTEX_STAGE] = shadow_vert;
    shader_code[ShaderStage::GEOMETRY_STAGE] = shadow_geom;
    shader_code[ShaderStage::FRAGMENT_STAGE] = shadow_frag;

    return CreateShaderProgram(shader_code);
}
} // namespace Shaders
