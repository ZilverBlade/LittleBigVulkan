#include "push_constant.h"
#include <cassert>

namespace LittleBigVulkan {
	LBVPushConstant::LBVPushConstant(size_t size, VkPipelineStageFlags stages, uint32_t offset) {
		assert(size <= 128, "Push Constant has a maximum recommended capacity of 128 bytes");
		pushConstantRange.stageFlags = stages;
		pushConstantRange.offset = offset;
		pushConstantRange.size = static_cast<uint32_t>(size);
	}
	LBVPushConstant::~LBVPushConstant() {
	}
	void LBVPushConstant::push(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, const void* data) {
		vkCmdPushConstants(
			commandBuffer,
			pipelineLayout,
			pushConstantRange.stageFlags,
			pushConstantRange.offset,
			pushConstantRange.size,
			data
		);
	}
}