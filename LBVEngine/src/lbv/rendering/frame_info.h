#pragma once
#include <lbv/dll.h>

#include <vulkan/vulkan.h>
#include <lbv/ecs/level.h>

namespace LittleBigVulkan {
	class ResourceSystem;
	struct FrameInfo {
		VkCommandBuffer commandBuffer;
		uint32_t frameIndex;
		float frameTime;
		VkDescriptorSet globalUBO;
		VkDescriptorSet sceneSSBO;
		std::shared_ptr<Level> level;
		ResourceSystem* resourceSystem;
	};
}