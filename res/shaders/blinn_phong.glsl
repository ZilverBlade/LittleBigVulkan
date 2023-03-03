#ifndef BLINN_PHONG_GLSL
#define BLINN_PHONG_GLSL
#extension GL_GOOGLE_include_directive : enable
#include "global_ubo.glsl"
#include "scene_ssbo.glsl"

struct FragmentData{
	vec3 normal;
	vec3 position;
	vec3 diffuse;
	float specular;
	float shininess;
};

float specularBlinnPhong(float NdH, float shininess) {
	return pow(NdH, shininess);
}

vec3 shadeBlinnPhong(FragmentData fragment) {
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	diffuse += scene.skyLight.tint.rgb * scene.skyLight.tint.a;
	
	vec3 cameraPosWorld = ubo.invViewMatrix[3].xyz;
	vec3 N = normalize(fragment.normal);
	vec3 V = normalize(cameraPosWorld - fragment.position);
	
	// directional light calc
	vec3 dL = -scene.directionalLight.direction;
	vec3 dRadiance = scene.directionalLight.color.rgb * scene.directionalLight.color.a;	
	vec3 dH = normalize(V + dL);
	diffuse += max(dot(N, dL), 0.0) * dRadiance;
	specular += specularBlinnPhong(max(dot(N, dH), 0.0), fragment.shininess) * dRadiance;
	
	return diffuse * fragment.diffuse + specular * fragment.specular;
}


#endif