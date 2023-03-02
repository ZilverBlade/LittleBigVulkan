#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/framebuffer_attachment.h>

namespace LittleBigVulkan {
	class LBVAPI LBVRenderPass;
	class LBVAPI LBVFramebuffer {
	public:
		LBVFramebuffer(LBVDevice& device, LBVRenderPass* renderPass, const std::vector<LBVFramebufferAttachment*>& attachments, uint32_t layer = 0, uint32_t mipLevel = 0);
		~LBVFramebuffer();

		LBVFramebuffer(const LBVFramebuffer&) = delete;
		LBVFramebuffer& operator=(const LBVFramebuffer&) = delete;
		LBVFramebuffer(LBVFramebuffer&&) = delete;
		LBVFramebuffer& operator=(LBVFramebuffer&&) = delete;

		inline glm::ivec3 getDimensions() { return { width, height, depth }; }
		inline VkFramebuffer getFramebuffer() { return framebuffer; }
		void resize(glm::ivec3 newDimensions, LBVRenderPass* renderPass);
	private:
		void create(LBVDevice& device, VkRenderPass renderPass, const std::vector<LBVFramebufferAttachment*>& attachments);
		void destroy();

		uint32_t width{};
		uint32_t height{};
		uint32_t depth{};

		VkFramebuffer framebuffer{};
		uint32_t ulbvMipLevel = 0;
		uint32_t ulbvLayer = 0;

		std::vector<LBVFramebufferAttachment*> attachments;
		LBVDevice& lbvDevice;
	};
}