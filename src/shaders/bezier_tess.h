#pragma once

#include <string_view>

constexpr std::string_view bezier_vert = R"(#version 430
layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

layout (location = 0) out vec2 frag_coord;
layout (location = 2) out vec3 frag_light;
layout (location = 3) out vec3 frag_eye;

layout (location = 0) uniform mat4 model_view;
layout (location = 1) uniform mat4 projection;
layout (location = 2) uniform vec3 light_position;

void main() {
    vec3 pos = (model_view * vec4(coord, 1.0)).xyz;
    frag_light = normalize(light_position - pos);
    frag_eye = normalize(-pos);

    // frag_normal = normalize((model_view*vec4(normal, 0.0)).xyz);
    gl_Position = projection * vec4(pos, 1.0);
    frag_coord = texture_coord;
}
)";

constexpr std::string_view bezier_tcs = R"(#version 430
layout (location = 9) uniform float outer02;
layout (location = 10) uniform float outer13;
layout (location = 11) uniform float inner0;
layout (location = 12) uniform float inner1;

layout(vertices = 16)  out;

void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = outer02;
	gl_TessLevelOuter[2] = outer02;
	gl_TessLevelOuter[1] = outer13;
	gl_TessLevelOuter[3] = outer13;

	gl_TessLevelInner[0]  = inner0;
	gl_TessLevelInner[1]  = inner1;
}
)";

constexpr std::string_view bezier_tes = R"(#version 430
layout(quads, equal_spacing, ccw)  in;

layout (location = 1) out vec3 frag_normal;

void main() {
	vec4 p00 = gl_in[ 0].gl_Position;
	vec4 p10 = gl_in[ 1].gl_Position;
	vec4 p20 = gl_in[ 2].gl_Position;
	vec4 p30 = gl_in[ 3].gl_Position;
	vec4 p01 = gl_in[ 4].gl_Position;
	vec4 p11 = gl_in[ 5].gl_Position;
	vec4 p21 = gl_in[ 6].gl_Position;
	vec4 p31 = gl_in[ 7].gl_Position;
	vec4 p02 = gl_in[ 8].gl_Position;
	vec4 p12 = gl_in[ 9].gl_Position;
	vec4 p22 = gl_in[10].gl_Position;
	vec4 p32 = gl_in[11].gl_Position;
	vec4 p03 = gl_in[12].gl_Position;
	vec4 p13 = gl_in[13].gl_Position;
	vec4 p23 = gl_in[14].gl_Position;
	vec4 p33 = gl_in[15].gl_Position;
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	// the basis functions:
	float bu0 = (1.-u) * (1.-u) * (1.-u);
	float bu1 = 3. * u * (1.-u) * (1.-u);
	float bu2 = 3. * u * u * (1.-u);
	float bu3 = u * u * u;
	float dbu0 = -3. * (1.-u) * (1.-u);
	float dbu1 =  3. * (1.-u) * (1.-3.*u);
	float dbu2 =  3. * u *      (2.-3.*u);
	float dbu3 =  3. * u *      u;
	float bv0 = (1.-v) * (1.-v) * (1.-v);
	float bv1 = 3. * v * (1.-v) * (1.-v);
	float bv2 = 3. * v * v * (1.-v);
	float bv3 = v * v * v;
	float dbv0 = -3. * (1.-v) * (1.-v);
	float dbv1 =  3. * (1.-v) * (1.-3.*v);
	float dbv2 =  3. * v *      (2.-3.*v);
	float dbv3 =  3. * v *      v;
	// finally, we get to compute something:
	gl_Position =
	bu0 * (bv0*p00 + bv1*p01 + bv2*p02 + bv3*p03)
	+ bu1 * (bv0*p10 + bv1*p11 + bv2*p12 + bv3*p13)
	+ bu2 * (bv0*p20 + bv1*p21 + bv2*p22 + bv3*p23)
	+ bu3 * (bv0*p30 + bv1*p31 + bv2*p32 + bv3*p33);


	vec4 dpdu =  dbu0 *  ( bv0*p00 + bv1*p01 + bv2*p02 + bv3*p03 )
	+ dbu1 * ( bv0*p10 + bv1*p11 + bv2*p12 + bv3*p13 )
	+ dbu2 * ( bv0*p20 + bv1*p21 + bv2*p22 + bv3*p23 )
	+ dbu3 * ( bv0*p30 + bv1*p31 + bv2*p32 + bv3*p33 );
	vec4 dpdv =  bu0 *  ( dbv0*p00 + dbv1*p01 + dbv2*p02 + dbv3*p03 )
	+ bu1 * ( dbv0*p10 + dbv1*p11 + dbv2*p12 + dbv3*p13 )
	+ bu2 * ( dbv0*p20 + dbv1*p21 + dbv2*p22 + dbv3*p23 )
	+ bu3 * ( dbv0*p30 + dbv1*p31 + dbv2*p32 + dbv3*p33 );

	frag_normal = normalize( cross( dpdu.xyz, dpdv.xyz ) );
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
    vec3 model_diffuse = vec3(1.0, 0.5, 0.2);

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
