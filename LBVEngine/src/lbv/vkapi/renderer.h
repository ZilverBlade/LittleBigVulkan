#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/device.h>
#include <lbv/vkapi/command_buffer.h>

namespace LittleBigVulkan {
	class LBVAPI LBVRenderer {
	public:
		LBVRenderer(LBVDevice& device);
		~LBVRenderer();

		LBVRenderer(const LBVRenderer&) = delete;
		LBVRenderer& operator=(const LBVRenderer&) = delete;
		LBVRenderer(LBVRenderer&&) = delete;
		LBVRenderer& operator=(LBVRenderer&&) = delete;

		void submitCommandBuffers(const std::vector<LBVCommandBuffer*>& commandBuffers);
		void submitQueue(VkSubmitInfo submitInfo, VkFence fence);
		VkQueue getQueue() {
			return lbvQueue->queue;
		}
	private:
		std::vector<VkCommandBuffer> toSubmitCommandBuffers{};
		VkCommandPool commandPool;
		LBVDevice& lbvDevice;
		LBVQueue* lbvQueue;
	};
}