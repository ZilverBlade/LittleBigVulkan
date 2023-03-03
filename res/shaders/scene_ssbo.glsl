#ifndef SCENE_SSBO_GLSL
#define SCENE_SSBO_GLSL

struct SkyLight {
	vec4 tint; // w = intensity
};

struct DirectionalLight {
	vec3 direction;
	vec4 color; // w = intensity
};
struct PointLight {
	vec3 position;
	vec4 color; // w = intensity
};

layout (set = 1, binding = 0) readonly buffer SceneSSBO {
	SkyLight skyLight;
	DirectionalLight directionalLight;
	PointLight pointLights[256];
	uint pointLightCount;
} scene;
#endif