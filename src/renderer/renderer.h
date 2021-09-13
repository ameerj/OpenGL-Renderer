#pragma once

#include "../common/common_types.h"

namespace Renderer {
void ProcessInput(GLFWwindow* window);
void FbSizeCallback(GLFWwindow* window, int width, int height);
void DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar* message, const void* userParam);

GLFWwindow* InitWindow(u32 width, u32 height);
} // namespace Renderer
