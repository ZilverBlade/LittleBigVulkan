#pragma once
#include <lbv/dll.h>

#include <string>
#include <lbv/utils/uuid.h>
#include <glm/glm.hpp>

namespace LittleBigVulkan {
	inline namespace Components {
		struct TransformComponent {
			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;

			glm::vec3 translation{ 0.0f, 0.0f, 0.0f };
			glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
			glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

			glm::mat4 getTransformMatrix();
			glm::mat3 getNormalMatrix();
		};
	}
}