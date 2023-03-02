#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/device.h>
#include <glm/glm.hpp>

namespace LittleBigVulkan {
	enum class LBVAPI LBVFramebufferAttachmentType {
		Depth,
		Color,
		Resolve
	};

	struct LBVFramebufferAttachmentCreateInfo {
		VkFormat framebufferFormat;
		VkImageAspectFlags imageAspect;
		VkImageViewType viewType;
		glm::ivec3 dimensions;
		VkImageUsageFlags usage;
		VkImageLayout layout;
		LBVFramebufferAttachmentType framebufferType;
		bool linearFiltering = true;
		VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
		uint32_t mipLevels = 1;
		uint32_t layerCount = 1;
	};

	class LBVAPI LBVFramebufferAttachment {
	public:
		LBVFramebufferAttachment(LBVDevice& device, const LBVFramebufferAttachmentCreateInfo& attachmentCreateInfo);
		~LBVFramebufferAttachment();

		LBVFramebufferAttachment(const LBVFramebufferAttachment&) = delete;
		LBVFramebufferAttachment& operator=(const LBVFramebufferAttachment&) = delete;
		LBVFramebufferAttachment(LBVFramebufferAttachment&&) = delete;
		LBVFramebufferAttachment& operator=(LBVFramebufferAttachment&&) = delete;

		VkImage getImage() { return image; }
		VkImageSubresourceRange getImageSubresourceRange() { return subresourceRange; }
		VkImageView getImageView() { return imageView; }
		VkImageView getSubImageView(uint32_t layer, uint32_t mipLevel) {
			if (subImageViews.empty()) { // if layerCount == 1 and mipLevels == 1, there is no need for special image views for renderpaslbvs
				return imageView;
			}
			return subImageViews[layer][mipLevel];
		}
		VkSampler getSampler() { return sampler; }
		VkImageLayout getImageLayout() { return attachmentDescription.layout; }
		VkDeviceMemory getDeviceMemory() { return imageMemory; }
		glm::ivec3 getDimensions() { return dimensions; }
		VkDescriptorImageInfo getDescriptor() {
			return VkDescriptorImageInfo{
				sampler,
				imageView,
				attachmentDescription.layout
			};
		}
		LBVFramebufferAttachmentType getAttachmentType() { return attachmentDescription.framebufferType; }
		const LBVFramebufferAttachmentCreateInfo& getAttachmentDescription() { return attachmentDescription; }

		void resize(glm::ivec3 newDimensions);
	private:
		void destroy();
		void create(LBVDevice& device, const LBVFramebufferAttachmentCreateInfo& attachmentCreateInfo);

		VkImage image{};
		VkDeviceMemory imageMemory{};
		VkSampler sampler{};
		VkImageSubresourceRange subresourceRange{};
		VkImageView imageView{};

		std::vector<std::vector<VkImageView>> subImageViews{};
		std::vector<std::vector<VkImageSubresourceRange>> subSubresourceRanges{};

		LBVFramebufferAttachmentCreateInfo attachmentDescription;

		LBVDevice& lbvDevice;

		glm::ivec3 dimensions{};
	};
}