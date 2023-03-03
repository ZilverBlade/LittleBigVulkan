#pragma once
#include <lbv/dll.h>

#include <glm/glm.hpp>

namespace LittleBigVulkan {
	inline namespace Components {
		struct SkyComponent {
			glm::vec3 lightEmission{1.0f};
			float lightIntensity{1.2f};
			glm::vec3 environmentColor{ 0.85, 0.92, 1.0f };
			float environmentIntensity{ 0.5f };
		};
	}
}