#pragma once
#include <lbv/dll.h>
#include <lbv/vkapi/device.h>
namespace LittleBigVulkan {
	enum class LBVAPI CommandBufferLevel {
		Primary,
		Secondary
	};
	class LBVAPI LBVCommandBuffer {
	public:
		LBVCommandBuffer(LBVDevice& device, CommandBufferLevel level = CommandBufferLevel::Primary);
		~LBVCommandBuffer();
		LBVCommandBuffer(const LBVCommandBuffer&) = delete;
		LBVCommandBuffer& operator=(const LBVCommandBuffer&) = delete;

		VkCommandBuffer getCommandBuffer() {
			return commandBuffer;
		}
		void begin(VkCommandBufferUsageFlags flags = VkFlags());
		void end();
		void reset(VkCommandBufferResetFlags flags = VkFlags());
	private:
		VkCommandBuffer commandBuffer{};
		VkCommandPool commandPool;
		LBVDevice& lbvDevice;
	};

}