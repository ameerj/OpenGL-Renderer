#pragma once
#include <iostream>
#include <glad/glad.h>

class Shader {
public:
    Shader() = default;
    Shader(Shader&& rhs) : handle{std::exchange(rhs.handle, 0)} {}

    ~Shader() {
        Release();
    }

    void Release();

    GLuint handle = 0;
};

class Program {
public:
    Program() = default;
    Program(Program&& rhs) : handle{std::exchange(rhs.handle, 0)} {}

    ~Program() {
        Release();
    }

    void Release();

    GLuint handle = 0;
};

class Buffer {
public:
    Buffer() = default;
    Buffer(Buffer&& rhs) : handle{std::exchange(rhs.handle, 0)} {}

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
    VAO(VAO&& rhs) : handle{std::exchange(rhs.handle, 0)} {}

    ~VAO() {
        Release();
    }

    void Create();

    void Release();

    GLuint handle = 0;
};

class Sampler {
public:
    Sampler() = default;
    Sampler(Sampler&& rhs) : handle{std::exchange(rhs.handle, 0)} {}

    ~Sampler() {
        Release();
    }

    void Create();

    void Release();

    void DefaultConfiguration();

    GLuint handle = 0;
};

class Texture {
public:
    Texture() = default;
    Texture(Texture&& rhs) : handle{std::exchange(rhs.handle, 0)} {}

    ~Texture() {
        Release();
    }

    void Create();

    void Release();

    void UploadDataFromPath(const std::string& path);

    GLuint handle = 0;
};
