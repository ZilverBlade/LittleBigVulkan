#include "framebuffer.h"
#include <lbv/vkapi/render_pass.h>

namespace LittleBigVulkan{

	LBVFramebuffer::LBVFramebuffer(LBVDevice& device, LBVRenderPass* renderPass, const std::vector<LBVFramebufferAttachment*>& attachments, uint32_t layer, uint32_t mipLevel) : lbvDevice(device), ulbvLayer(layer), ulbvMipLevel(mipLevel) {
		create(device, renderPass->getRenderPass(), attachments);
		this->attachments.reserve(attachments.size());
		for (LBVFramebufferAttachment* attachment : attachments) {
			this->attachments.push_back(attachment);
		}
	}

	LBVFramebuffer::~LBVFramebuffer() {
		destroy();
	}

	void LBVFramebuffer::create(LBVDevice& device, VkRenderPass renderPass, const std::vector<LBVFramebufferAttachment*>& newAttachments) {
		std::vector<VkImageView> imageViews;
		for (auto& attachment : newAttachments) {
			imageViews.push_back(attachment->getSubImageView(ulbvLayer, ulbvMipLevel));
		}

		width = static_cast<uint32_t>(newAttachments[0]->getDimensions().x) / std::pow(2, ulbvMipLevel);
		height = static_cast<uint32_t>(newAttachments[0]->getDimensions().y) / std::pow(2, ulbvMipLevel);
		depth = static_cast<uint32_t>(newAttachments[0]->getDimensions().z);

		VkFramebufferCreateInfo fbufferCreateInfo{};
		fbufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufferCreateInfo.pNext = nullptr;
		fbufferCreateInfo.flags = 0;
		fbufferCreateInfo.renderPass = renderPass;
		fbufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
		fbufferCreateInfo.pAttachments = imageViews.data();
		fbufferCreateInfo.width = width;
		fbufferCreateInfo.height = height;
		fbufferCreateInfo.layers = 1;
		if (vkCreateFramebuffer(device.getDevice(), &fbufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer!");
		}
	}

	void LBVFramebuffer::destroy() {
		vkDestroyFramebuffer(lbvDevice.getDevice(), framebuffer, nullptr);
	}

	void LBVFramebuffer::resize(glm::ivec3 newDimensions, LBVRenderPass* renderPass) {
		for (LBVFramebufferAttachment* attachment : attachments) {
			attachment->resize(newDimensions);
		}
		destroy();
		create(lbvDevice, renderPass->getRenderPass(), attachments);
	}

}