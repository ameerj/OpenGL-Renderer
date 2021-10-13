#pragma once

#include <string_view>

constexpr std::string_view explode_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

layout (location = 0) out vec2 vs_coord;
layout (location = 1) out vec3 vs_normal;
layout (location = 2) out vec3 vs_light;
layout (location = 3) out vec3 vs_eye;

layout (location = 0) uniform mat4 model_view;
layout (location = 2) uniform vec3 light_position;

void main() {
    vec3 pos = (model_view * vec4(coord, 1.0)).xyz;
    gl_Position = vec4(pos, 1.0);

    vs_light = normalize(light_position - pos);
    vs_eye = normalize(-pos);
    vs_normal = normalize((model_view*vec4(normal, 0.0)).xyz);;
    vs_coord = texture_coord;
}
)";

constexpr std::string_view explode_geom = R"(#version 430
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec2 vs_coord[];
layout (location = 1) in vec3 vs_normal[];
layout (location = 2) in vec3 vs_light[];
layout (location = 3) in vec3 vs_eye[];

layout (location = 0) out vec2 frag_coord;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec3 frag_light;
layout (location = 3) out vec3 frag_eye;

layout (location = 1) uniform mat4 projection;
layout (location = 9) uniform float time;

void main() {
    vec3 side_0 = (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 side_1 = (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 normal = normalize(cross(side_1, side_0));

    for (int i = 0; i < 3; ++i) {
        vec3 offset = normal * ((sin(time) + 1.0) / 4.0);
        gl_Position = projection * gl_in[i].gl_Position + vec4(offset, 1);
        frag_coord = vs_coord[i];
        frag_normal = vs_normal[i];
        frag_light = vs_light[i];
        frag_eye = vs_eye[i];
        EmitVertex();
    }
    EndPrimitive();
}
)";

constexpr std::string_view explode_frag = R"(#version 430
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
