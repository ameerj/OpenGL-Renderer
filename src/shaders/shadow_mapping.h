#pragma once

#include <string_view>

constexpr std::string_view shadow_vert = R"(#version 430
layout (location = 0) in vec3 coord;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;

void main() {
    gl_Position = view * model * vec4(coord, 1.0);
}
)";

constexpr std::string_view shadow_geom = R"(#version 430
layout (triangles) in;

// Max vertices 3 verts per tri, * 6 faces = 18
layout (triangle_strip, max_vertices = 18) out;

layout (location = 16) uniform mat4 shadow_matrices[6];

out vec4 frag_pos;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) {
            vec4 vert_pos = gl_in[i].gl_Position;
            frag_pos = vert_pos;
            gl_Position = shadow_matrices[face] * vert_pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}  
)";

constexpr std::string_view shadow_frag = R"(#version 430
in vec4 frag_pos;

layout (location = 2) uniform vec3 light_pos;

void main() {
    // get distance between fragment and light source
    float light_distance = length(frag_pos.xyz - light_pos);
    
    // map to [0:1] range by dividing by far_plane
    light_distance /= 25.0f;
    
    // write this as modified depth
    gl_FragDepth = light_distance;
}  
)";
