#pragma once
#include "light_system.h"
#include <glm/gtx/rotate_vector.hpp>


namespace LittleBigVulkan {
	void LightSystem::update(FrameInfo& frameInfo, SceneSSBO& sceneBuffer) {
		uint32_t directionalLightIndex = 0;
		frameInfo.level->getRegistry().view<Components::TransformComponent, Components::SkyComponent>().each(
			[&](auto& transform, auto& sky) {
				glm::mat4 tm = transform.getTransformMatrix();
				tm = glm::rotate(tm, glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f));
				glm::vec3 direction = glm::normalize(tm[2]); // convert rotation to direction

				sceneBuffer.directionalLight.direction = direction;
				sceneBuffer.directionalLight.color = { sky.lightEmission, sky.lightIntensity };
				sceneBuffer.skyLight.tint = { sky.environmentColor, sky.environmentIntensity };
			}
		);

		uint32_t pointLightIndex = 0;
		frameInfo.level->getRegistry().view<Components::TransformComponent, Components::PointLightComponent>().each(
			[&](auto& transform, auto& light) {
				glm::mat4 tm = transform.getTransformMatrix();
				sceneBuffer.pointLights[pointLightIndex].position = glm::vec3(tm[3]);
				sceneBuffer.pointLights[pointLightIndex].color = { light.emission, light.intensity };
				pointLightIndex++;
			}
		);
		sceneBuffer.pointLightCount = pointLightIndex;
	}
}