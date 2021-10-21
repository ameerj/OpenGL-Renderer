#pragma once

#include "../resources/resources.h"

namespace Shaders {

Program GetRasterShader();

Program GetPhongShader();

Program GetBezierShader();

Program GetGeometryShader();

Program GetGeometryBezierShader();

Program GetTfbShader();

} // namespace Shaders
