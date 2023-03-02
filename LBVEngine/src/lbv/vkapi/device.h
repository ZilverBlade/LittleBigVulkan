#pragma once
#include <lbv/dll.h>

#include <vector>
#include <lbv/log.h>
#include <lbv/vkapi/queue.h>

namespace LittleBigVulkan {
	struct QueueFamilyIndices {
		uint32_t prelbvntFamily = 0;
		uint32_t prelbvntQueueCount = 0;

		uint32_t graphicsFamily = -1;
		uint32_t graphicsQueueCount = 0;

		uint32_t computeFamily = -1;
		uint32_t computeQueueCount = 0;
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class LBVAPI LBVDevice {
	public:
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
	};
#ifndef NDEBUG
		const bool enableValidationLayers = true;
#else
		const bool enableValidationLayers = false;
#endif

		LBVDevice(const LBVDevice&) = delete;
		LBVDevice& operator=(const LBVDevice&) = delete;
		LBVDevice(LBVDevice&&) = delete;
		LBVDevice& operator=(LBVDevice&&) = delete;

		LBVDevice();
		~LBVDevice();

		VkInstance getInstance() {
			return instance;
		}
		VkDevice getDevice() {
			return device;
		}
		VkPhysicalDevice getPhysicalDevice() {
			return physicalDevice;
		}

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		SwapChainSupportDetails getSwapChainSupport(VkSurfaceKHR surface);

		VkCommandPool getCommandPool() {
			return commandPool;
		}

		LBVQueue* getAvailableQueue(LBVQueueType type);
		void freeAvailableQueue(LBVQueue* queue);

		const VkPhysicalDeviceProperties& getDeviceProperties() { return deviceProperties; }
		const VkPhysicalDeviceFeatures& getDeviceFeatures() { return deviceFeatures; }
		QueueFamilyIndices findPhysicalQueueFamilies() {
			return findQueueFamilies(physicalDevice);
		}

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);
		void copyImage(VkCommandBuffer commandBuffer, VkExtent3D extent, VkImage srcImage, VkImageLayout srcLayout, VkImageSubresourceLayers srcSubresourceLayers, VkOffset3D srcOffset, VkImage dstImage, VkImageLayout dstLayout, VkImageSubresourceLayers dstSubresourceLayers, VkOffset3D dstOffset);
		void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
		void copyImageToBuffer(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout layout, VkBuffer buffer, uint32_t width, uint32_t height, uint32_t layerCount, uint32_t mipLevel);
		void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount);
		void generateMipMaps(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layerCount, VkImageLayout finalLayout);		
	private:

		VkPhysicalDeviceProperties deviceProperties{};
		VkPhysicalDeviceFeatures deviceFeatures{};

		bool checkValidationLayerSupport();

		void createInstance();
		void pickPhyisicalDevice();
		void createLogicalDevice();
		void createCommandPool();

		VkPhysicalDeviceFeatures requestFeatures();
		bool checkDeviceFeatureSupport(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		bool isDeviceSuitable(VkPhysicalDevice device);

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();

		std::vector<LBVQueue> createQueues(LBVQueueType queueType, uint32_t queueCount, uint32_t queueOffset);

		void setupDebugMeslbvnger();

		VkInstance instance{};
		VkDevice device{};
		VkPhysicalDevice physicalDevice{};

		std::vector<LBVQueue> graphicsQueues{};
		std::vector<LBVQueue> computeQueues{};

		VkCommandPool commandPool;

		VkDebugUtilsMessengerEXT debugMessenger{};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};
}