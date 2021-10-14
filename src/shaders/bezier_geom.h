#pragma once

#include <string_view>

constexpr std::string_view bezier_geom = R"(#version 430
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec2 tess_coord[];
layout (location = 1) in vec3 tess_normal[];
layout (location = 2) in vec3 tess_light[];
layout (location = 3) in vec3 tess_eye[];

layout (location = 0) out vec2 frag_coord;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec3 frag_light;
layout (location = 3) out vec3 frag_eye;

layout (location = 13) uniform float time;

void main() {
    vec3 side_0 = (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 side_1 = (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 normal = normalize(cross(side_1, side_0));

    for (int i = 0; i < 3; ++i) {
        vec3 offset = normal * ((sin(time * 1.5) + 1.0) / 2.0);
        gl_Position = (gl_in[i].gl_Position + vec4(offset, 1));
        frag_coord = tess_coord[i];
        frag_normal = tess_normal[i];
        frag_light = tess_light[i];
        frag_eye = tess_eye[i];
        EmitVertex();
    }
    EndPrimitive();
}
)";
