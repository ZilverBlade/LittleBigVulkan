#version 450
#extension GL_GOOGLE_include_directive : enable
#define SURFACE_MATERIAL_DESCRIPTOR_SET 3
#include "surface_material.glsl"
#include "normal_mapping.glsl"

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec2 fragUV;
layout (location = 2) in vec3 fragNormal;
layout (location = 3) in vec3 fragTangent;

void main() {
	vec2 uv = sfSampleUV(fragUV);
	if (sfHasMaskTexture()){
		if (texture(sfMaskTexture, uv).x < 0.5) {
			discard;
		}
	}
}