#version 450
#extension GL_GOOGLE_include_directive : enable
#include "blinn_phong.glsl"

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec2 fragUV;
layout (location = 2) in vec3 fragNormal;

void main() {
	FragmentData fragment;
	fragment.normal = normalize(fragNormal);
	fragment.position = fragPosWorld;
	fragment.diffuse = vec3(0.5, 0.5, 0.5);
	fragment.specular = 0.5;
	fragment.shininess = 64.0;

	vec3 result = shadeBlinnPhong(fragment);
	outColor = vec4(result, 1.0);
}