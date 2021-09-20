#include "resources.h"

#define DEFAULT_MOVE_CONSTRUCTOR                                                                   \
    handle = rhs.handle;                                                                           \
    rhs.handle = 0;

Shader::Shader(Shader&& rhs) {
    DEFAULT_MOVE_CONSTRUCTOR;
}

void Shader::Release() {
    if (handle == 0)
        return;
    glDeleteShader(handle);
    handle = 0;
}

Program::Program(Program&& rhs) {
    DEFAULT_MOVE_CONSTRUCTOR;
}

void Program::Release() {
    if (handle == 0)
        return;
    glDeleteProgram(handle);
    handle = 0;
}

Buffer::Buffer(Buffer&& rhs) {
    DEFAULT_MOVE_CONSTRUCTOR;
}

void Buffer::Create() {
    if (handle != 0)
        return;
    glCreateBuffers(1, &handle);
}

void Buffer::Release() {
    if (handle == 0)
        return;
    glDeleteBuffers(1, &handle);
    handle = 0;
}

VAO::VAO(VAO&& rhs) {
    DEFAULT_MOVE_CONSTRUCTOR;
}

void VAO::Create() {
    if (handle != 0)
        return;
    glGenVertexArrays(1, &handle);
}

void VAO::Release() {
    if (handle == 0)
        return;
    glDeleteVertexArrays(1, &handle);
    handle = 0;
}
