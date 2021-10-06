#define GLFW_INCLUDE_NONE
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "../common/common_types.h"
#include "basic_rasterizer.h"
#include "bezier_tess.h"
#include "phong_shading.h"
#include "shaders.h"

namespace Shaders {
namespace {
Shader CompileShader(GLenum stage, const char* shader_src) {
    Shader shader;
    shader.handle = glCreateShader(stage);
    glShaderSource(shader.handle, 1, &shader_src, NULL);
    glCompileShader(shader.handle);

    return shader;
}

void LinkProgram(GLuint handle) {
    glLinkProgram(handle);

    GLint link_status{};
    glGetProgramiv(handle, GL_LINK_STATUS, &link_status);

    GLint log_length{};
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length == 0) {
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
} // namespace

Program GetRasterShader() {
    const char* vertex_shader_code = basic_vert.data();
    const char* fragment_shader_code = basic_frag.data();

    const Shader vertex = CompileShader(GL_VERTEX_SHADER, vertex_shader_code);
    const Shader fragment = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    Program program;
    program.handle = glCreateProgram();

    glAttachShader(program.handle, vertex.handle);
    glAttachShader(program.handle, fragment.handle);
    LinkProgram(program.handle);

    return program;
}

Program GetPhongShader() {
    const char* vertex_shader_code = phong_vert.data();
    const char* fragment_shader_code = phong_frag.data();

    const Shader vertex = CompileShader(GL_VERTEX_SHADER, vertex_shader_code);
    const Shader fragment = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    Program program;
    program.handle = glCreateProgram();

    glAttachShader(program.handle, vertex.handle);
    glAttachShader(program.handle, fragment.handle);
    LinkProgram(program.handle);

    return program;
}

Program GetBezierShader() {
    const char* vertex_shader_code = bezier_vert.data();
    const char* tess_control_shader_code = bezier_tcs.data();
    const char* tess_eval_shader_code = bezier_tes.data();
    const char* fragment_shader_code = bezier_frag.data();

    const Shader vertex = CompileShader(GL_VERTEX_SHADER, vertex_shader_code);
    const Shader tcs = CompileShader(GL_TESS_CONTROL_SHADER, tess_control_shader_code);
    const Shader tes = CompileShader(GL_TESS_EVALUATION_SHADER, tess_eval_shader_code);
    const Shader fragment = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    Program program;
    program.handle = glCreateProgram();

    glAttachShader(program.handle, vertex.handle);
    glAttachShader(program.handle, tcs.handle);
    glAttachShader(program.handle, tes.handle);
    glAttachShader(program.handle, fragment.handle);
    LinkProgram(program.handle);

    return program;
}
} // namespace Shaders
