#pragma once

#include <string_view>

constexpr std::string_view basic_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;
layout (location = 0) out vec2 frag_coord;

layout (location = 0) uniform mat4 mvp;


void main() {
    gl_Position = mvp * vec4(coord, 1.0f);
    frag_coord = texture_coord;
}
)";

constexpr std::string_view basic_frag = R"(#version 430
layout (location = 0) in vec2 frag_coord;
layout (location = 0) out vec4 color;

uniform sampler2D diffuse;

void main() {
    color = texture(diffuse, frag_coord);
}
)";
