#pragma once

#include <string_view>

constexpr std::string_view fullscreen_vert = R"(#version 430
layout(location = 0) out vec2 tex_coord;

void main() {
    float x = float((gl_VertexID & 1) << 2);
    float y = float((gl_VertexID & 2) << 1);
    gl_Position = vec4(x - 1.0, y - 1.0, 0.0, 1.0);
    tex_coord = vec2(x, y) / 2.0;
}

)";

constexpr std::string_view fullscreen_frag = R"(#version 430
layout (location = 0) in vec2 frag_tex_coord;
layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D color_texture;

void main() {
    color = vec4(texture(color_texture, frag_tex_coord).rgb, 1.0f);
}

)";
