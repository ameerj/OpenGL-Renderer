#define GLFW_INCLUDE_NONE
#include <algorithm>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>

#include "common/common_types.h"
#include "renderer/renderer.h"

int main() {
    constexpr u32 width{1280};
    constexpr u32 height{720};

    Renderer::Renderer renderer(width, height);
    renderer.SetMeshModel("../res/models/sonic.obj");
    renderer.RenderLoop();

    return 0;
}
