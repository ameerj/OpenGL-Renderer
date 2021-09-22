#pragma once

#include <string_view>

constexpr std::string_view quad_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 0) out vec3 frag_color;

layout (location = 0) uniform mat4 mvp;


void main() {
    gl_Position = mvp * vec4(coord, 1.0f);
    frag_color = coord + vec3(0.5);
}
)";

constexpr std::string_view quad_frag = R"(#version 430
layout (location = 0) in vec3 frag_color;
layout (location = 0) out vec4 color;

void main() {
    color = vec4(frag_color, 1.0);
}
)";
