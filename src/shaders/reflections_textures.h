#pragma once

#include <string_view>

constexpr std::string_view ssr_texture_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

layout (location = 0) out vec2 frag_coord;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec3 frag_pos;
layout (location = 3) out vec3 vert_view_pos;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

void main() {
    vec4 vert_coord = vec4(coord, 1.0f);
    frag_pos = vec3(model * vert_coord);
    frag_normal = transpose(inverse(mat3(model))) * normal;
    frag_coord = texture_coord;

    vert_view_pos = vec3(view * model * vert_coord);
    gl_Position = projection * view * model * vert_coord;
}
)";

constexpr std::string_view ssr_texture_frag = R"(#version 430
layout (location = 0) in vec2 frag_coord;
layout (location = 1) in vec3 frag_normal;
layout (location = 2) in vec3 frag_pos;
layout (location = 3) in vec3 vert_view_pos;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 normal_texture;
layout (location = 2) out vec4 positions_texture;

layout (binding = 0) uniform sampler2D diffuse_texture;

layout (location = 3) uniform vec3 model_ambient;
layout (location = 4) uniform vec3 model_specular;
layout (location = 5) uniform float model_shininess;

layout (location = 6) uniform vec3 light_ambient;
layout (location = 7) uniform vec3 light_diffuse;
layout (location = 8) uniform vec3 light_specular;

layout (location = 9) uniform vec3 view_position;
layout (location = 10) uniform vec3 light_position[2];

void main() {
    vec3 model_diffuse = texture(diffuse_texture, frag_coord).rgb;
    color = vec4(0);

    vec3 N = normalize(frag_normal);
    vec3 E = normalize(view_position - frag_pos);

    for (int i = 0; i < 2; ++i) {
        vec3 L = normalize(light_position[i] - frag_pos);
        vec3 H = normalize(L + E);

        vec3 ambient = model_ambient * light_ambient;
        float diffuseTerm = max(dot(L, N), 0.0);
        vec3  diffuse = diffuseTerm*(model_diffuse * light_diffuse);
        float specularTerm = pow(max(dot(N, H), 0.0), model_shininess);
        vec3  specular = specularTerm * (model_specular * light_specular);
    
        if (dot(L, N) < 0.0) {
            specular = vec3(0.0, 0.0, 0.0);
        }
        color += vec4(diffuse + specular + ambient, 1.0f);
    }
    normal_texture = vec4(N, 1.0f);
    positions_texture = vec4(vert_view_pos, 1.0f);
}
)";
