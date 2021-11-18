#pragma once

#include <string_view>

constexpr std::string_view reflect_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

layout (location = 0) out vec2 vert_coord;
layout (location = 1) out vec3 vert_normal;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;

void main() {
    gl_Position = view * model * vec4(coord, 1.0);
    vert_coord = texture_coord;
    vert_normal = normal;
}
)";

constexpr std::string_view reflect_geom = R"(#version 430
layout (triangles) in;

// Max vertices 3 verts per tri, * 6 faces = 18
layout (triangle_strip, max_vertices = 18) out;

layout (location = 16) uniform mat4 projection_matrices[6];


layout (location = 0) in vec2 vert_coord[];
layout (location = 1) in vec3 vert_normal[];

layout (location = 0) out vec2 frag_coord;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec3 frag_pos;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) {
            vec4 vert_pos = gl_in[i].gl_Position;
            gl_Position = projection_matrices[face] * vert_pos;
            frag_coord = vert_coord[i];
            frag_normal = vert_normal[i];
            frag_pos = vert_pos.xyz;
            EmitVertex();
        }
        EndPrimitive();
    }
}  
)";
