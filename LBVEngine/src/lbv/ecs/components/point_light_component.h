#pragma once
#include <lbv/dll.h>

#include <glm/glm.hpp>

namespace LittleBigVulkan {
	inline namespace Components {
		struct PointLightComponent {
			glm::vec3 emission{1.0f};
			float intensity{1.0f};
		};
	}
}