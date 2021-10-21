#pragma once

#include <string_view>

constexpr std::string_view tfb_vert = R"(#version 430
layout (location = 0) in vec3 coord;

layout (location = 0) out vec3 transformed_coord;

layout (location = 0) uniform vec2 click_pos;

void main() {
    gl_PointSize = 5.0f;
    transformed_coord = coord + vec3((click_pos - coord.xy) / 10.0f, 0.0f);
    gl_Position = vec4(transformed_coord, 1.0f);
}
)";

constexpr std::string_view tfb_frag = R"(#version 430
layout (location = 0) out vec4 color;

void main() {
    color = vec4(0.0, 0.7, 0.3, 1.0);
}
)";
