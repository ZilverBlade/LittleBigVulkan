#include "command_buffer.h"

namespace LittleBigVulkan {
	LBVCommandBuffer::LBVCommandBuffer(LBVDevice& device, CommandBufferLevel level) : commandPool(device.getCommandPool()), lbvDevice(device) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;
		if (level == CommandBufferLevel::Primary) allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(lbvDevice.getDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}
	LBVCommandBuffer::~LBVCommandBuffer() {
		vkFreeCommandBuffers(
			lbvDevice.getDevice(),
			commandPool,
			1,
			&commandBuffer
		);
	}
	void LBVCommandBuffer::begin(VkCommandBufferUsageFlags flags) {
		VkCommandBufferBeginInfo beginInfo{};
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // ulbvful for renderpass only commands
		beginInfo.flags = flags;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}
	void LBVCommandBuffer::end() {
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
	void LBVCommandBuffer::reset(VkCommandBufferResetFlags flags) {
		if (vkResetCommandBuffer(commandBuffer, flags) != VK_SUCCESS) {
			throw std::runtime_error("failed to reset command buffer!");
		}
	}
}