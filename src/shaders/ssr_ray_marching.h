#pragma once

#include <string_view>

constexpr std::string_view ssr_march_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

layout (location = 0) out vec2 frag_coord;
layout (location = 1) out vec3 frag_normal;
layout (location = 2) out vec3 frag_pos;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

void main() {
    vec4 vert_coord = vec4(coord, 1.0f);
    frag_pos = vec3(model * vert_coord);
    frag_normal = transpose(inverse(mat3(model))) * normal;
    frag_coord = texture_coord;

    gl_Position = projection * view * model * vert_coord;
}
)";

constexpr std::string_view ssr_march_frag = R"(#version 430
layout (location = 0) in vec2 frag_coord;
layout (location = 1) in vec3 frag_normal;
layout (location = 2) in vec3 frag_pos;

layout (location = 0) out vec4 color;

layout (location = 2) uniform mat4 projection;

layout (binding = 0) uniform sampler2D position_texture;
layout (binding = 1) uniform sampler2D normal_texture;
layout (binding = 2) uniform sampler2D color_texture;

out vec4 fragColor;

void main() {
    float max_distance = 8;
    float resolution = 0.3;
    int steps = 5;
    float thickness = 0.5;

    vec2 tex_size = textureSize(position_texture, 0).xy;
    vec2 tex_coord = gl_FragCoord.xy / tex_size;

    vec4 uv = vec4(0.0);

    vec4 position_from = texture(position_texture, tex_coord);
    if (position_from.w <= 0.0) {
        color = texture(color_texture, uv.xy);
        return;
    }

    vec3 unit_position_from = normalize(position_from.xyz);
    vec3 normal = normalize(texture(normal_texture, tex_coord).xyz);
    vec3 pivot = normalize(reflect(unit_position_from, normal));

    vec4 position_to = position_from;

    vec4 start_view = vec4(position_from.xyz + (pivot * 0.0), 1.0);
    vec4 end_view = vec4(position_from.xyz + (pivot * max_distance), 1.0);

    vec4 start_frag = start_view;
    start_frag = projection * start_frag;
    start_frag.xyz /= start_frag.w;
    start_frag.xy = start_frag.xy * 0.5 + 0.5;
    start_frag.xy *= tex_size;

    vec4 end_frag = end_view;
    end_frag = projection * end_frag;
    end_frag.xyz /= end_frag.w;
    end_frag.xy = end_frag.xy * 0.5 + 0.5;
    end_frag.xy *= tex_size;

    vec2 frag = start_frag.xy;
    uv.xy = frag / tex_size;

    float delta_x = end_frag.x - start_frag.x;
    float delta_y = end_frag.y - start_frag.y;
    float useX = abs(delta_x) >= abs(delta_y) ? 1.0 : 0.0;
    float delta = mix(abs(delta_y), abs(delta_x), useX) * clamp(resolution, 0.0, 1.0);
    vec2 increment = vec2(delta_x, delta_y) / max(delta, 0.001);

    float search0 = 0;
    float search1 = 0;

    int hit0 = 0;
    int hit1 = 0;

    float view_distance = start_view.y;
    float depth = thickness;

    float i = 0;

    for (i = 0; i < int(delta); ++i) {
        frag += increment;
        uv.xy = frag / tex_size;
        position_to = texture(position_texture, uv.xy);

        search1 = mix((frag.y - start_frag.y) / delta_y, (frag.x - start_frag.x) / delta_x, useX);

        search1 = clamp(search1, 0.0, 1.0);

        view_distance = (start_view.y * end_view.y) / mix(end_view.y, start_view.y, search1);
        depth = view_distance - position_to.y;

        if (depth > 0 && depth < thickness) {
            hit0 = 1;
            break;
        } else {
            search0 = search1;
        }
    }

    search1 = search0 + ((search1 - search0) / 2.0);

    steps *= hit0;

    for (i = 0; i < steps; ++i) {
        frag = mix(start_frag.xy, end_frag.xy, search1);
        uv.xy = frag / tex_size;
        position_to = texture(position_texture, uv.xy);

        view_distance = (start_view.y * end_view.y) / mix(end_view.y, start_view.y, search1);
        depth = view_distance - position_to.y;

        if (depth > 0 && depth < thickness) {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) / 2);
        } else {
            float temp = search1;
            search1 = search1 + ((search1 - search0) / 2);
            search0 = temp;
        }
    }

    float visibility = hit1 * position_to.w * (1 - max(dot(-unit_position_from, pivot), 0)) *
                       (1 - clamp(depth / thickness, 0, 1)) *
                       (1 - clamp(length(position_to - position_from) / max_distance, 0, 1)) *
                       (uv.x < 0 || uv.x > 1 ? 0 : 1) * (uv.y < 0 || uv.y > 1 ? 0 : 1);

    visibility = clamp(visibility, 0, 1);

    uv.ba = vec2(visibility);

    color = uv;

    color = texture(color_texture, uv.xy);
}
)";
