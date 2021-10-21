#pragma once

#include <string_view>

constexpr std::string_view tfb_vert = R"(#version 430
layout (location = 0) in vec3 coord;


void main() {
    gl_PointSize = 5.0f;
    gl_Position = vec4(coord, 1.0f);
}
)";

constexpr std::string_view tfb_frag = R"(#version 430
layout (location = 0) out vec4 color;

void main() {
    color = vec4(0.0, 0.7, 0.3, 1.0);
}
)";
