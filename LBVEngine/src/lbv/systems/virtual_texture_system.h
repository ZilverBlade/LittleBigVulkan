#pragma once
#include <lbv/dll.h>

#include <unordered_map>
#include <lbv/vk.h>
#include <lbv/graphics/texture2d.h>
#include <unordered_set>

namespace LittleBigVulkan {
	struct VirtualTextureResource {
		VkOffset2D offset{};
		VkExtent2D size{};
	};

	class LBVAPI VirtualTexture {
	public:
		VirtualTexture(LBVDevice& device, LBVDescriptorPool& descriptorPool, VkExtent2D size, VkFormat format);
		~VirtualTexture();

		VirtualTexture(const VirtualTexture&) = delete;
		VirtualTexture& operator=(const VirtualTexture&) = delete;
	private:
		void createImage();
		void createImageView();
		void createSampler();

		VkImage virtualTexture;
		VkDeviceMemory virtualTextureMemory;
		VkImageView virtualImageView;
		VkSampler virtualSampler;

		VkExtent2D virtualTextureSize;
		VkFormat virtualTextureFormat;

		std::unordered_set<VirtualTextureResource> virtualResources;

		LBVDevice& lbvDevice;
		LBVDescriptorPool& lbvDescriptorPool;

		friend class VirtualTextureAllocator;
	};

	class LBVAPI VirtualTextureAllocator {
	public:
		struct VirtualTextureResourceCopyInfo {
			VirtualTextureResource resourceData;
			VkImage image;
			VkImageSubresourceLayers subresource;
		};

		VirtualTextureAllocator(VirtualTexture& virtualTexture);
		~VirtualTextureAllocator();

		VirtualTextureAllocator(const VirtualTextureAllocator&) = delete;
		VirtualTextureAllocator& operator=(const VirtualTextureAllocator&) = delete;

		bool isColliding(VirtualTextureResource resource);
		VirtualTextureResource findFreeSlot(VkExtent2D size);
		VirtualTextureAllocator& free(VirtualTextureResource resourceData);
		VirtualTextureAllocator& insert(VirtualTextureResource resourceData, VkImage image, VkImageSubresource subresource, VkImageAspectFlags aspect);

		void copyTextures(VkCommandBuffer commandBuffer);
	private:
		std::vector<VirtualTextureResourceCopyInfo> resourceCopyInfos{};
;		VirtualTexture& virtualTexture;
	};
}