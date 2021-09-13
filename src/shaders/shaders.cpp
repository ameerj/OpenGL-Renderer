#define GLFW_INCLUDE_NONE
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "../common/common_types.h"
#include "quad_program.h"
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
} // namespace

Program GetRasterShader() {
    const char* vertex_shader_code = quad_vert.data();
    const char* fragment_shader_code = quad_frag.data();

    const Shader vertex = CompileShader(GL_VERTEX_SHADER, vertex_shader_code);
    const Shader fragment = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    Program program;
    program.handle = glCreateProgram();

    glAttachShader(program.handle, vertex.handle);
    glAttachShader(program.handle, fragment.handle);
    glLinkProgram(program.handle);

    return program;
}

GLuint BindQuadBuffers() {
    constexpr std::array vertices{
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    };

    u32 vertex_buffer_object, vertex_array_object;
    glGenVertexArrays(1, &vertex_array_object);
    glGenBuffers(1, &vertex_buffer_object);
    glBindVertexArray(vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    return vertex_array_object;
}
} // namespace Shaders
