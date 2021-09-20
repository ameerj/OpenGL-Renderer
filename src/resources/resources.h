#pragma once
#include <iostream>
#include <glad/glad.h>

class Shader {
public:
    Shader() = default;
    Shader(Shader&& rhs);

    ~Shader() {
        Release();
    }

    void Release();

    GLuint handle = 0;
};

class Program {
public:
    Program() = default;
    Program(Program&& rhs);

    ~Program() {
        Release();
    }

    void Release();

    GLuint handle = 0;
};

class Buffer {
public:
    Buffer() = default;
    Buffer(Buffer&& rhs);

    ~Buffer() {
        Release();
    }

    void Create();

    void Release();

    GLuint handle = 0;
};

class VAO {
public:
    VAO() = default;
    VAO(VAO&& rhs);

    ~VAO() {
        Release();
    }

    void Create();

    void Release();

    GLuint handle = 0;
};
