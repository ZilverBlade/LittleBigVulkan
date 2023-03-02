#include "framebuffer_attachment.h"
namespace LittleBigVulkan {
	LBVFramebufferAttachment::LBVFramebufferAttachment(LBVDevice& device, const LBVFramebufferAttachmentCreateInfo& attachmentCreateInfo) : lbvDevice(device), attachmentDescription(attachmentCreateInfo) {
		create(device, attachmentCreateInfo);
	}
	LBVFramebufferAttachment::~LBVFramebufferAttachment() {
		destroy();
	}

	void LBVFramebufferAttachment::create(LBVDevice& device, const LBVFramebufferAttachmentCreateInfo& attachmentCreateInfo) {
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = attachmentCreateInfo.framebufferFormat;
		imageCreateInfo.extent.width = attachmentCreateInfo.dimensions.x;
		imageCreateInfo.extent.height = attachmentCreateInfo.dimensions.y;
		imageCreateInfo.extent.depth = attachmentCreateInfo.dimensions.z;
		imageCreateInfo.mipLevels = attachmentCreateInfo.mipLevels;
		imageCreateInfo.arrayLayers = attachmentCreateInfo.layerCount;
		imageCreateInfo.samples = attachmentCreateInfo.sampleCount;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = attachmentCreateInfo.usage;
		imageCreateInfo.flags |= attachmentCreateInfo.viewType == VK_IMAGE_VIEW_TYPE_CUBE ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
		VkMemoryAllocateInfo memAlloc = {};
		memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements memReqs;

		if (vkCreateImage(device.getDevice(), &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		vkGetImageMemoryRequirements(device.getDevice(), image, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = device.findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device.getDevice(), &memAlloc, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate memory!");
		}

		if (vkBindImageMemory(device.getDevice(), image, imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind memory!");
		}
		
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = attachmentCreateInfo.viewType;
		imageViewCreateInfo.format = attachmentCreateInfo.framebufferFormat;
		subresourceRange.aspectMask = attachmentCreateInfo.imageAspect;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = attachmentCreateInfo.mipLevels;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = attachmentCreateInfo.layerCount;
		imageViewCreateInfo.subresourceRange = subresourceRange;
		imageViewCreateInfo.image = image;

		if (vkCreateImageView(device.getDevice(), &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image view!");
		}
		if (attachmentCreateInfo.layerCount > 1 || attachmentCreateInfo.mipLevels > 1) {
			subImageViews.resize(attachmentCreateInfo.layerCount);
			subSubresourceRanges.resize(attachmentCreateInfo.layerCount);
			for (int i = 0; i < attachmentCreateInfo.layerCount; i++) {
				subImageViews[i].resize(attachmentCreateInfo.mipLevels);
				subSubresourceRanges[i].resize(attachmentCreateInfo.mipLevels);
				for (int j = 0; j < attachmentCreateInfo.mipLevels; j++) {
					VkImageViewCreateInfo subImageViewCreateInfo = {};
					subImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					subImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
					subImageViewCreateInfo.format = attachmentCreateInfo.framebufferFormat;
					subSubresourceRanges[i][j].aspectMask = attachmentCreateInfo.imageAspect;
					subSubresourceRanges[i][j].baseMipLevel = j;
					subSubresourceRanges[i][j].levelCount = 1;
					subSubresourceRanges[i][j].baseArrayLayer = i;
					subSubresourceRanges[i][j].layerCount = 1;
					subImageViewCreateInfo.subresourceRange = subSubresourceRanges[i][j];
					subImageViewCreateInfo.image = image;

					if (vkCreateImageView(device.getDevice(), &subImageViewCreateInfo, nullptr, &subImageViews[i][j]) != VK_SUCCESS) {
						throw std::runtime_error("Failed to create image view!");
					}
				}
			}
		}	

		if (attachmentCreateInfo.usage & VK_IMAGE_USAGE_SAMPLED_BIT) {
			// Create sampler to sample from the attachment in the fragment shader
			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = attachmentCreateInfo.linearFiltering ? VK_FILTER_LINEAR :  VK_FILTER_NEAREST;
			samplerInfo.minFilter = attachmentCreateInfo.linearFiltering ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
			samplerInfo.mipmapMode = attachmentCreateInfo.linearFiltering ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.maxAnisotropy = 1.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = static_cast<float>(attachmentCreateInfo.mipLevels);
			samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			samplerInfo.addressModeV = samplerInfo.addressModeU;
			samplerInfo.addressModeW = samplerInfo.addressModeU;
			if (vkCreateSampler(device.getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create sampler");
			}
		}
		dimensions = { attachmentCreateInfo.dimensions.x, attachmentCreateInfo.dimensions.y, attachmentCreateInfo.dimensions.z };
	}
	void LBVFramebufferAttachment::destroy() {
		vkDestroyImageView(lbvDevice.getDevice(), imageView, nullptr);
		if (attachmentDescription.layerCount > 1 || attachmentDescription.mipLevels > 1) {
			for (int i = 0; i < attachmentDescription.layerCount; i++) {
				for (int j = 0; j < attachmentDescription.mipLevels; j++) {
					vkDestroyImageView(lbvDevice.getDevice(), subImageViews[i][j], nullptr);
				}
			}
		}	
		vkDestroySampler(lbvDevice.getDevice(), sampler, nullptr);
		vkDestroyImage(lbvDevice.getDevice(), image, nullptr);
		vkFreeMemory(lbvDevice.getDevice(), imageMemory, nullptr);
	}

	void LBVFramebufferAttachment::resize(glm::ivec3 newDimensions) {
		if (newDimensions == attachmentDescription.dimensions) return; // in case already been resized
		destroy();
		attachmentDescription.dimensions = newDimensions;
		create(lbvDevice, attachmentDescription);
	}	
}
