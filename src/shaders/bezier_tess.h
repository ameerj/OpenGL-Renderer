#pragma once

#include <string_view>

constexpr std::string_view bezier_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

layout (location = 0) out vec2 vs_coord;
layout (location = 2) out vec3 vs_light;
layout (location = 3) out vec3 vs_eye;

layout (location = 0) uniform mat4 model_view;
layout (location = 1) uniform mat4 projection;
layout (location = 2) uniform vec3 light_position;

void main() {
    vec3 pos = (model_view * vec4(coord, 1.0)).xyz;
    vs_light = normalize(light_position - pos);
    vs_eye = normalize(-pos);

    // frag_normal = normalize((model_view*vec4(normal, 0.0)).xyz);
    gl_Position = projection * vec4(pos, 1.0);
    vs_coord = texture_coord;
}
)";

constexpr std::string_view bezier_tcs = R"(#version 430
layout (location = 9) uniform float outer02;
layout (location = 10) uniform float outer13;
layout (location = 11) uniform float inner0;
layout (location = 12) uniform float inner1;

layout (location = 0) in vec2 vs_coord[];
layout (location = 2) in vec3 vs_light[];
layout (location = 3) in vec3 vs_eye[];

layout(vertices = 16) out;

patch out vec2 patch_coord;
patch out vec3 patch_light;
patch out vec3 patch_eye;

void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = outer02;
	gl_TessLevelOuter[2] = outer02;
	gl_TessLevelOuter[1] = outer13;
	gl_TessLevelOuter[3] = outer13;

	gl_TessLevelInner[0]  = inner0;
	gl_TessLevelInner[1]  = inner1;

	// Pass through the VS attributes
	patch_coord = vs_coord[gl_InvocationID];
	patch_light = vs_light[gl_InvocationID];
	patch_eye = vs_eye[gl_InvocationID];
}
)";

constexpr std::string_view bezier_tes = R"(#version 430
layout(quads, equal_spacing, ccw)  in;

patch in vec2 patch_coord;
patch in vec3 patch_light;
patch in vec3 patch_eye;

layout (location = 0) out vec2 frag_coord;
layout (location = 2) out vec3 frag_light;
layout (location = 3) out vec3 frag_eye;

layout (location = 1) out vec3 frag_normal;

// Basis functions
float BFunc(int i, float u) {
	switch (i) {
	case 0:
		return pow(1.0f - u, 3);
	case 1:
		return 3.0f * u * (pow((1.0f - u), 2));
	case 2:
        return 3.0f * (pow(u, 2) * (1.0f - u));
	case 3:
        return pow(u, 3);
	}
}

// Derivative functions
float DFunc(int i, float u) {
	switch (i) {
	case 0:
		return -3.0f * pow(1.0f - u, 2);
	case 1:
		return 3.0f * (1.0f - u) * (1.0f - 3.0f * u);
	case 2:
        return 3.0f * u * (2.0f - 3.0f * u);
	case 3:
        return 3.0f * pow(u, 2);
	}
}

void main() {
	// Bezier u, v parameters:
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	gl_Position = vec4(0);
	vec4 dpdu = vec4(0);
	vec4 dpdv = vec4(0);

    for (int i = 0; i < 4; i++) {
		float bu = BFunc(i, u);
		float du = DFunc(i, u);
        for (int j = 0; j < 4; j++) {
			float bv = BFunc(j, v);
			float dv = DFunc(j, v);
			vec4 control_point = gl_in[i * 4 + j].gl_Position;

            gl_Position += bu * bv * control_point;
			dpdu +=		   du * bv * control_point;
			dpdv +=		   bu * dv * control_point;
        }
    }

	frag_normal = normalize(cross(dpdu.xyz, dpdv.xyz));
	frag_coord = patch_coord;
	frag_light = patch_light;
	frag_eye = patch_eye;
}
)";

constexpr std::string_view bezier_frag = R"(#version 430
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
    vec3 model_diffuse = vec3(0.50, 1.0, 1.0);

    vec3 N = -normalize(frag_normal);
    vec3 E = normalize(frag_eye);
    vec3 L = normalize(frag_light);
    vec3 H = normalize(L + E);
    if( dot(L, N) < 0.0 ) {
        N = -1.0 * N;
    }

    vec3 ambient = model_ambient * light_ambient;
    float diffuseTerm = max(dot(L, N), 0.0);
    vec3  diffuse = diffuseTerm*(model_diffuse * light_diffuse);
    float specularTerm = pow(max(dot(N, H), 0.0), model_shininess);
    vec3  specular = specularTerm * (model_specular * light_specular);
    color = vec4(ambient + diffuse + specular, 1.0f);
}
)";
