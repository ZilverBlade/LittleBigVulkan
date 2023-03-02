#ifndef SURFACE_MATERIAL_GLSL
#define SURFACE_MATERIAL_GLSL

//#define SURFACE_MATERIAL_DESCRIPTOR_SET N

const uint SURFACE_MATERIAL_SHADING_MODEL_LIT = 0x01;
const uint SURFACE_MATERIAL_SHADING_MODEL_UNLIT = 0x02;

const uint SURFACE_MATERIAL_TEXTURE_DIFFUSE_BIT = 0x01;
const uint SURFACE_MATERIAL_TEXTURE_EMISSIVE_BIT = 0x02;
const uint SURFACE_MATERIAL_TEXTURE_NORMAL_BIT = 0x04;
const uint SURFACE_MATERIAL_TEXTURE_SPECULAR_BIT = 0x08;
const uint SURFACE_MATERIAL_TEXTURE_MASK_BIT = 0x80;

layout(set = SURFACE_MATERIAL_DESCRIPTOR_SET, binding = 0) uniform Params {
	vec4 diffuse;
	vec4 emission;
	float specular;
	float shininess;
	vec2 uvScale;
	vec2 uvOffset;
	uint textureFlags;
	uint shadingModelFlag;
} surfaceMaterialParams;

layout(set = SURFACE_MATERIAL_DESCRIPTOR_SET, binding = 1) uniform sampler2D sfDiffuseTexture;
layout(set = SURFACE_MATERIAL_DESCRIPTOR_SET, binding = 2) uniform sampler2D sfEmissiveTexture;
layout(set = SURFACE_MATERIAL_DESCRIPTOR_SET, binding = 3) uniform sampler2D sfNormalTexture;
layout(set = SURFACE_MATERIAL_DESCRIPTOR_SET, binding = 4) uniform sampler2D sfSpecularTexture;
layout(set = SURFACE_MATERIAL_DESCRIPTOR_SET, binding = 5) uniform sampler2D sfMaskTexture;
//layout(set = SURFACE_MATERIAL_DESCRIPTOR_SET, binding = 9) uniform sampler2D sfOpacityTexture;

bool sfHasDiffuseTexture() {
	return bool((surfaceMaterialParams.textureFlags & SURFACE_MATERIAL_TEXTURE_DIFFUSE_BIT) != 0);
}
bool sfHasEmissiveTexture() {
	return bool((surfaceMaterialParams.textureFlags & SURFACE_MATERIAL_TEXTURE_EMISSIVE_BIT) != 0);
}
bool sfHasNormalTexture() {
	return bool((surfaceMaterialParams.textureFlags & SURFACE_MATERIAL_TEXTURE_NORMAL_BIT) != 0);
}
bool sfHasSpecularTexture() {
	return bool((surfaceMaterialParams.textureFlags & SURFACE_MATERIAL_TEXTURE_SPECULAR_BIT) != 0);
}
bool sfHasMaskTexture() {
	return bool((surfaceMaterialParams.textureFlags & SURFACE_MATERIAL_TEXTURE_MASK_BIT) != 0);
}

bool sfShadingModelLit() {
	return (surfaceMaterialParams.shadingModelFlag == SURFACE_MATERIAL_SHADING_MODEL_LIT);
}
bool sfShadingModelUnlit() {
	return (surfaceMaterialParams.shadingModelFlag == SURFACE_MATERIAL_SHADING_MODEL_UNLIT);
}

vec2 sfSampleUV(vec2 uv) {
	return uv * surfaceMaterialParams.uvScale + surfaceMaterialParams.uvOffset;
}

#endif