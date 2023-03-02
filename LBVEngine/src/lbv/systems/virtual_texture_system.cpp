#include "virtual_texture_system.h"

namespace LittleBigVulkan {
	VirtualTexture::VirtualTexture(LBVDevice& device, LBVDescriptorPool& descriptorPool, VkExtent2D size, VkFormat format) 
		: lbvDevice(device), lbvDescriptorPool(descriptorPool), virtualTextureSize(size), virtualTextureFormat(format) {
	
		createImage();
		createImageView();
		createSampler();
	}
	VirtualTexture::~VirtualTexture() {
		vkDestroyImage(lbvDevice.getDevice(), virtualTexture, nullptr);
		vkFreeMemory(lbvDevice.getDevice(), virtualTextureMemory, nullptr);
		vkDestroyImageView(lbvDevice.getDevice(), virtualImageView, nullptr);
		vkDestroySampler(lbvDevice.getDevice(), virtualSampler, nullptr);
	}
	void VirtualTexture::createImage() {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent = { virtualTextureSize.width, virtualTextureSize.height, 1 };
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = virtualTextureFormat;
		imageInfo.flags = 0;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		lbvDevice.createImageWithInfo(
			imageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			virtualTexture,
			virtualTextureMemory
		);
	}
	void VirtualTexture::createImageView() {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = virtualTexture;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = virtualTextureFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

		if (vkCreateImageView(lbvDevice.getDevice(), &viewInfo, nullptr, &virtualImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create virtual texture image view!");
		}
	}
	void VirtualTexture::createSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		samplerInfo.anisotropyEnable = false;
		samplerInfo.maxAnisotropy = 1.0;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 1.0f;

		if (vkCreateSampler(lbvDevice.getDevice(), &samplerInfo, nullptr, &virtualSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create virtual texture sampler!");
		}
	}
	VirtualTextureAllocator::VirtualTextureAllocator(VirtualTexture& virtualTexture) : virtualTexture(virtualTexture) {

	}
	VirtualTextureAllocator::~VirtualTextureAllocator() {
	
	}
	bool VirtualTextureAllocator::isColliding(VirtualTextureResource resource) {
		for (auto& vr : virtualTexture.virtualResources) {
			glm::vec2 resourceMin = { resource.offset.x, resource.offset.y };
			glm::vec2 resourceMax = resourceMin + glm::vec2{ resource.size.width, resource.size.height };

			glm::vec2 vrMin = { vr.offset.x, vr.offset.y };
			glm::vec2 vrMax = resourceMin + glm::vec2{ vr.size.width, vr.size.height };

			bool isTouching = glm::all(glm::greaterThan(resourceMin, vrMin) || glm::lessThan(resourceMax, vrMax));
			if (isTouching) {
				LBVLOG_WRN("Resource at [min(%f, %f) max(%f, %f)], is colliding with resource at [min(%f, %f) max(%f, %f)]!",
					resourceMin.x, resourceMin.y, resourceMax.x, resourceMax.y,
					vrMin.x, vrMin.y, vrMax.x, vrMax.y
				);
				return true;
			}
		}
		return false;
	}
	VirtualTextureResource VirtualTextureAllocator::findFreeSlot(VkExtent2D size) {
		VirtualTextureResource resource{};
		resource.size = size;

		VkOffset2D offset = { 0, 0 };
		for (auto& vr : virtualTexture.virtualResources) {
			glm::vec2 resourceMin = { offset.x, offset.y };
			glm::vec2 resourceMax = resourceMin + glm::vec2{ size.width, size.height };

			glm::vec2 vrMin = { vr.offset.x, vr.offset.y };
			glm::vec2 vrMax = resourceMin + glm::vec2{ vr.size.width, vr.size.height };

			// in hindsight this probably isnt effective as it might shift the slot to the previously compared slot

			glm::bvec2 isTouching = glm::greaterThan(resourceMin, vrMin) || glm::lessThan(resourceMax, vrMax);
			if (isTouching.x && offset.x < vrMax.x) {
				offset.x = vrMax.x;
			}
			if (isTouching.y && offset.y < vrMax.y) {
				offset.y = vrMax.y;
			}
		}
		resource.offset = offset;
		return resource;
	}
	VirtualTextureAllocator& VirtualTextureAllocator::free(VirtualTextureResource resourceData) {
		auto iter = virtualTexture.virtualResources.find(resourceData);
		if (iter == virtualTexture.virtualResources.end()) {
			LBVLOG_ERR("Allocated resource does not exist! Nothing will be freed");
		} else {
			virtualTexture.virtualResources.erase(iter);
		}
		return *this;
	}
	VirtualTextureAllocator& VirtualTextureAllocator::insert(VirtualTextureResource resourceData, VkImage image, VkImageSubresource subresource, VkImageAspectFlags aspect) {
		VirtualTextureResourceCopyInfo copyInfo{};
		copyInfo.image = image;
		copyInfo.resourceData = resourceData;

		VkImageSubresourceLayers subresourceLayers{};
		subresourceLayers.aspectMask = subresource.aspectMask;
		subresourceLayers.baseArrayLayer = subresource.arrayLayer;
		subresourceLayers.layerCount = 1;
		subresourceLayers.mipLevel = 0;
		copyInfo.subresource = subresourceLayers;

		resourceCopyInfos.push_back(std::move(copyInfo));
		return *this;
	}
	void VirtualTextureAllocator::copyTextures(VkCommandBuffer commandBuffer) {
		VkImageSubresourceLayers virtualTextureSubresourceLayers{};
		virtualTextureSubresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		virtualTextureSubresourceLayers.baseArrayLayer = 0;
		virtualTextureSubresourceLayers.layerCount = 1;
		virtualTextureSubresourceLayers.mipLevel = 0;

		for (auto& copy : resourceCopyInfos) {
			VkExtent3D extent = { copy.resourceData.size.width, copy.resourceData.size.height, 1 };
			VkOffset3D offset = { copy.resourceData.offset.x, copy.resourceData.offset.y, 0 };
			virtualTexture.virtualResources.emplace(copy.resourceData);
			virtualTexture.lbvDevice.copyImage(
				commandBuffer, extent,
				copy.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, copy.subresource, VkOffset3D(),
				virtualTexture.virtualTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, virtualTextureSubresourceLayers, offset
			);
		}
	}
}