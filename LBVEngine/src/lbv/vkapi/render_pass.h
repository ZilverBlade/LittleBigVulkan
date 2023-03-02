#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/framebuffer.h>
#include <lbv/vkapi/framebuffer_attachment.h>

namespace LittleBigVulkan {
	struct LBVAttachmentClearColor {
		VkClearColorValue color{{0.f, 0.f, 0.f, 1.f}};
		VkClearDepthStencilValue depth{ 1.f, 0 };
	};
	struct LBVAttachmentInfo {
		LBVFramebufferAttachment* framebufferAttachment;
		VkAttachmentLoadOp loadOp;
		VkAttachmentStoreOp storeOp;
		LBVAttachmentClearColor clear = LBVAttachmentClearColor{};
	};

	class LBVAPI LBVRenderPass {
	public:
		LBVRenderPass(LBVDevice& device, const std::vector<LBVAttachmentInfo>& attachments);
		~LBVRenderPass();

		LBVRenderPass(const LBVRenderPass&) = delete;
		LBVRenderPass& operator=(const LBVRenderPass&) = delete;
		LBVRenderPass(LBVRenderPass&&) = delete;
		LBVRenderPass& operator=(LBVRenderPass&&) = delete;

		void beginRenderPass(VkCommandBuffer commandbuffer, LBVFramebuffer* framebuffer);
		void endRenderPass(VkCommandBuffer commandbuffer);

		void setViewportSize(uint32_t w, uint32_t h) { width = w; height = h; }

		VkRenderPass getRenderPass() { return renderpass; }
	private:
		uint32_t width{};
		uint32_t height{};

		LBVDevice& lbvDevice;

		std::vector<VkClearValue> clearValues{};
		VkRenderPass renderpass{};
	};
}