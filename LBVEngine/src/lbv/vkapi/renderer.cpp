#include "renderer.h"

namespace LittleBigVulkan {
	LBVRenderer::LBVRenderer(LBVDevice& device) : lbvDevice(device), commandPool(device.getCommandPool()) {
		lbvQueue = lbvDevice.getAvailableQueue(LBVQueueType::Graphics);
	}
	LBVRenderer::~LBVRenderer() {
		lbvDevice.freeAvailableQueue(lbvQueue);
	}
	void LBVRenderer::submitCommandBuffers(const std::vector<LBVCommandBuffer*>& commandBuffers) {
		for (LBVCommandBuffer* cb : commandBuffers) {
			toSubmitCommandBuffers.push_back(cb->getCommandBuffer());
		}
	}
	void LBVRenderer::submitQueue(VkSubmitInfo submitInfo, VkFence fence) {
		submitInfo.commandBufferCount = toSubmitCommandBuffers.size();
		submitInfo.pCommandBuffers = toSubmitCommandBuffers.data();
		if (VkResult result = vkQueueSubmit(lbvQueue->queue, 1, &submitInfo, fence); result != VK_SUCCESS) {
			throw std::runtime_error("failed to submit queue!");
		}
		toSubmitCommandBuffers.clear();
	}
}