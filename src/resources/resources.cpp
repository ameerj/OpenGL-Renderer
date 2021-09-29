#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>

#include "../common/common_types.h"
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

void Texture::Create() {
    if (handle != 0)
        return;
    glCreateTextures(GL_TEXTURE_2D, 1, &handle);
}

void Texture::Release() {
    if (handle == 0)
        return;
    glDeleteTextures(1, &handle);
    handle = 0;
}

void Texture::UploadDataFromPath(const std::string& path) {
    int width, height, bit_depth;
    u8* texture_data = stbi_load(path.c_str(), &width, &height, &bit_depth, 0);
    if (!texture_data) {
        printf("Failed to find: %s\n", path.c_str());
        return;
    }
    if (bit_depth == 3) {
        glTextureStorage2D(handle, 1, GL_RGB8, width, height);
    } else if (bit_depth == 4) {
        glTextureStorage2D(handle, 1, GL_RGBA8, width, height);
    } else {
        printf("Unimplemented texture bit depth: %d\n", bit_depth);
        glTextureStorage2D(handle, 1, GL_RGBA8, width, height);
    }
    glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
    glGenerateTextureMipmap(handle);

    stbi_image_free(texture_data);
}

void Sampler::Create() {
    if (handle != 0)
        return;
    glCreateSamplers(1, &handle);
}

void Sampler::Release() {
    if (handle == 0)
        return;
    glDeleteSamplers(1, &handle);
    handle = 0;
}

void Sampler::DefaultConfiguration() {
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
