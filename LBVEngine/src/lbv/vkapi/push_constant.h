#pragma once
#include <lbv/dll.h>

#include <vulkan/vulkan.h>

namespace LittleBigVulkan {
	class LBVAPI LBVPushConstant {
	public:
		LBVPushConstant() = default;
		LBVPushConstant(size_t size, VkPipelineStageFlags stages, uint32_t offset = 0);
		~LBVPushConstant();
		void push(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, const void* data);
		inline VkPushConstantRange getRange() { return pushConstantRange; }
	private:
		VkPushConstantRange pushConstantRange{};
	};
}