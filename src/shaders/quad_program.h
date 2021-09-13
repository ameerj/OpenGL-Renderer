#pragma once

#include <string_view>

constexpr std::string_view quad_vert = R"(#version 430
layout(location = 0) in vec2 coord;
layout (location = 0) out vec2 frag_tex_coord;

void main() {
    gl_Position = vec4(coord, 0.0f, 1.0f);
    frag_tex_coord = (coord + 1.0f) / 2.0f;
}
)";

constexpr std::string_view quad_frag = R"(#version 430
layout (location = 0) in vec2 frag_tex_coord;
layout (location = 0) out vec4 color;
// layout (binding = 0) uniform sampler2D color_texture;
layout(location = 2) uniform vec3 color_uniform;

void main() {
    // color = vec4(texture(color_texture, frag_tex_coord).rgb, 1.0f);
    color = vec4(color_uniform, 1.0);
}
)";
