#pragma once
#include <glad/glad.h>

class Shader {
public:
    Shader() = default;

    ~Shader() {
        Release();
    }

    void Release();

    GLuint handle = 0;
};

class Program {
public:
    Program() = default;

    ~Program() {
        Release();
    }

    void Release();

    GLuint handle = 0;
};

class Buffer {
public:
    Buffer() = default;

    ~Buffer() {
        Release();
    }

    void Create();

    void Release();

    GLuint handle = 0;
};
