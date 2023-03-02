#pragma once
#include <lbv/dll.h>

#include <glm/glm.hpp>
#include <lbv/utils/resid.h>

namespace LittleBigVulkan {
	inline namespace Components {
		struct SkyLightComponent {
			glm::vec3 tint{ 1.0f };
			float intensity{ 1.0f };
			ResourceID environmentMap{};
		};
	}
}