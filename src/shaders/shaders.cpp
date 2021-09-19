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

Buffer GetVertexBuffer() {
    constexpr std::array vertices{
        0.75f, -0.75f, -0.75f, -0.75f, 0.0f, 0.75f,
    };

    Buffer vertex_buffer_object;
    vertex_buffer_object.Create();
    glNamedBufferData(vertex_buffer_object.handle, sizeof(vertices), vertices.data(),
                      GL_STATIC_DRAW);

    return vertex_buffer_object;
}
} // namespace Shaders
