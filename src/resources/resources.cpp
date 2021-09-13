#include "resources.h"

void Shader::Release() {
    if (handle == 0)
        return;
    glDeleteShader(handle);
    handle = 0;
}

void Program::Release() {
    if (handle == 0)
        return;
    glDeleteProgram(handle);
    handle = 0;
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
