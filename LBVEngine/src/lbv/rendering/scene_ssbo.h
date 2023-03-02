#pragma once
#include <lbv/dll.h>

#include <glm/glm.hpp>

namespace LittleBigVulkan {
	struct SkyLight {
		alignas(16)glm::vec4 tint; // w = intensity
	};
	struct DirectionalLight {
		alignas(16)glm::vec3 direction;
		alignas(16)glm::vec4 color; // w = intensity
	};
	struct PointLight {
		alignas(16)glm::vec3 position;
		alignas(16)glm::vec4 color; // w = intensity
	};

	struct SceneSSBO {
		SkyLight skyLight;
		DirectionalLight directionalLight;
		PointLight pointLights[256];
		uint32_t pointLightCount;
	};
}