#pragma once

#include "../resources/resources.h"

namespace Shaders {

Program GetRasterShader();

Program GetPhongShader();

Program GetBezierShader();

Program GetGeometryShader();

Program GetGeometryBezierShader();

Program GetTfbShader();

Program GetMultiLightShader();

Program GetShadowMappingShader();

Program GetFullscreenShader();

Program GetSSRTexturesShader();

Program GetSSRRayMarchShader();

} // namespace Shaders
