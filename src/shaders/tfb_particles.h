#pragma once

#include <string_view>

constexpr std::string_view tfb_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

layout (location = 0) out vec3 transformed_coord;
layout (location = 1) out vec3 tfb_normal;
layout (location = 2) out vec2 tfb_tex_coord;

layout (location = 0) uniform vec2 click_pos;
layout (location = 1) uniform uint invert;

void main() {
    float click_distance = distance(click_pos, coord.xy);
    bool close = click_distance <= 0.5;
    float pull_factor = close ? 1.0 - click_distance : 1.0;

    vec3 offset = vec3((click_pos - coord.xy) / (700.0f * pull_factor), 0.0f);
    if (invert != 0) {
        transformed_coord = coord - offset;
    } else {
        transformed_coord = coord + offset;
    }

    gl_PointSize = 2.0f;
    gl_Position = vec4(transformed_coord, 1.0f);
    tfb_normal = normal;
    tfb_tex_coord = tex_coord;
}
)";

constexpr std::string_view tfb_frag = R"(#version 430
layout (location = 0) out vec4 color;

void main() {
    color = vec4(0.0, 0.7, 0.3, 1.0);
}
)";
