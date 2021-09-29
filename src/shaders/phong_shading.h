#pragma once

#include <string_view>

constexpr std::string_view phong_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

layout (location = 0) out vec2 frag_coord;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec3 frag_light;
layout (location = 3) out vec3 frag_eye;

layout (location = 0) uniform mat4 model_view;
layout (location = 1) uniform mat4 projection;
layout (location = 2) uniform vec3 light_position;

void main() {
    vec3 pos = (model_view * vec4(coord, 1.0)).xyz;
    frag_light = normalize(light_position - pos);
    frag_eye = normalize(-pos);

    frag_normal = normalize((model_view*vec4(normal, 0.0)).xyz);
    gl_Position = projection * vec4(pos, 1.0);
    frag_coord = texture_coord;
}
)";

constexpr std::string_view phong_frag = R"(#version 430
layout (location = 0) in vec2 frag_coord;
layout (location = 1) in vec3 frag_normal;
layout (location = 2) in vec3 frag_light;
layout (location = 3) in vec3 frag_eye;

layout (location = 0) out vec4 color;

uniform sampler2D diffuse_texture;
layout (location = 3) uniform vec3 model_ambient;
layout (location = 4) uniform vec3 model_specular;
layout (location = 5) uniform float model_shininess;

layout (location = 6) uniform vec3 light_ambient;
layout (location = 7) uniform vec3 light_diffuse;
layout (location = 8) uniform vec3 light_specular;

void main() {
    vec3 model_diffuse = texture(diffuse_texture, frag_coord).rgb;

    vec3 N = normalize(frag_normal);
    vec3 E = normalize(frag_eye);
    vec3 L = normalize(frag_light);
    vec3 H = normalize(L + E);

    vec3 ambient = model_ambient * light_ambient;
    float diffuseTerm = max(dot(L, N), 0.0);
    vec3  diffuse = diffuseTerm*(model_diffuse * light_diffuse);
    float specularTerm = pow(max(dot(N, H), 0.0), model_shininess);
    vec3  specular = specularTerm * (model_specular * light_specular);
    
    if (dot(L, N) < 0.0) {
        specular = vec3(0.0, 0.0, 0.0);
    }
    color = vec4(ambient + diffuse + specular, 1.0f);
}
)";
