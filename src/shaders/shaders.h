#pragma once

#include <glad/glad.h>

#include "../resources/resources.h"

namespace Shaders {

Program GetRasterShader();
GLuint BindQuadBuffers();

} // namespace Shaders
