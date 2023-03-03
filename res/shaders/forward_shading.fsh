#version 450
#extension GL_GOOGLE_include_directive : enable

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec2 fragUV;
layout (location = 2) in vec3 fragNormal;

void main() {
	outColor = vec4(fragNormal + sqrt(gl_FragCoord.z), .0);
}