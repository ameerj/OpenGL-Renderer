#pragma once

#include <string_view>

constexpr std::string_view reflect_sampling_frag = R"(#version 430
layout (location = 1) in vec3 frag_normal;
layout (location = 2) in vec3 frag_pos;

layout (location = 0) out vec4 color;

layout (location = 9) uniform vec3 view_position;


layout (binding = 0) uniform samplerCube reflection_texture;

void main() {
    vec3 N = normalize(frag_normal);
    vec3 E = normalize(view_position - frag_pos);
    vec3 reflection_vector = 2 * (dot(N, E)) * N - E;
    color = vec4(texture(reflection_texture, -reflection_vector).rgb, 1.0f);
}

)";
